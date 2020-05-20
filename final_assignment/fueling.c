/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "main.h"
#include "fuelsel.h"
#include "print.h"
#include "lcd.h"
#include "payment.h"
#include "fueling.h"



static SemaphoreHandle_t xSemaphoreFueling = NULL;
static INT16U pFuelingValue = 0;
float timerCallBackTimeSekunds = 1.0;
static TimerHandle_t fuelPulse = NULL;
int fuelingState = INIT;
float deliveredGas = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvFuelingTask( void *pvParameters );

/* getter and setters */
INT16U get_fueling(){
  INT16U fuelingVal = -1;
  if( xSemaphoreTake( xSemaphoreFueling, portMAX_DELAY ) ){
    fuelingVal = pFuelingValue;
    xSemaphoreGive( xSemaphoreFueling );
  }
  return fuelingVal; //-1 if error, value else
}

void callBackFunction(TimerHandle_t localFuelPulse)
{
    int callBackCount;

    callBackCount = (int)pvTimerGetTimerID(fuelPulse);
    callBackCount++; 
  

    switch (fuelingState)
    {
    case START_FUELING:
        deliveredGas += 0.05 / timerCallBackTimeSekunds;
        if (callBackCount >= (2 / timerCallBackTimeSekunds))
            fuelingState = FUELING;
        else
            vTimerSetTimerID(fuelPulse, (void*)callBackCount);
        break;
    case FUELING:
        deliveredGas += 0.3 / timerCallBackTimeSekunds;
        break;
    case END_FUELING:
        deliveredGas += 0.05 / timerCallBackTimeSekunds;
        if (callBackCount >= (5 / timerCallBackTimeSekunds))
        {
            fuelingState = FUELING_TERMINATE;
            uartPrint("Fueling has been terminated\r\n");
        }
        else
        {
            vTimerSetTimerID(fuelPulse, (void*)callBackCount);
            uartPrint("ved at være slut med fueling\r\n");
        }
        break;
    default:
        break;
    }
}

/*-----------------------------------------------------------*/
BOOLEAN init_fueling( void ){
    fuelPulse = xTimerCreate("Timer", pdMS_TO_TICKS(1000 * timerCallBackTimeSekunds ), pdTRUE, (void*) 0, callBackFunction) ;
  xSemaphoreFueling = xSemaphoreCreateMutex();
  if( xSemaphoreFueling != NULL ){
    xTaskCreate( prvFuelingTask, "fueling task", 200 , NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    uartPrint("fueling initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void setEVGroupPayment(EventGroupHandle_t localTaskEventGroup) {

    //if done - give to next task:
    EventBits_t uxBits;
    uxBits = xEventGroupClearBits(localTaskEventGroup, EV_GROUP_fueling); // clear current bits first
    if (uxBits != EV_GROUP_fueling) {
        uartPrint("ERROR: clear of EV_GROUP_fueling was not successful\r\n");
    }
    uxBits = xEventGroupSetBits(localTaskEventGroup, EV_GROUP_payment); // set bits for next task to be unblocked
    if (uxBits != EV_GROUP_payment) {
        uartPrint("ERROR: set of EV_GROUP_payment was not successful\r\n");
    }
}


static void prvFuelingTask( void *pvParameters )
{
    float priceDifferece = 0;
    float totalPrice = 0;
    int fuelType = 0;
    float fuelPrice = 0.0;
    int cashSum = 0;
    int paymentType = 0;
    char line1[16];
    char line2[16]; 
    INT8U fuelNozzle = 0;

  const TickType_t xBlockTime = pdMS_TO_TICKS( 50 );

  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_fueling, pdFALSE, pdTRUE, portMAX_DELAY );
    if(uxBits == EV_GROUP_fueling){

       
      switch (fuelingState)
      {
      case INIT: 
          GPIO_PORTF_DATA_R &= ledOFF;
          fuelType = getFuelTypeAndReset();
          fuelPrice = getPrice(fuelType);
          cashSum = getCashSum();
          paymentType = getPaymentType();
          fuelingState = IDLE;
          break;
      case IDLE:
          sendToLcd(" Lift nozzle to ", " start fueling");
          if (!nozzleBoot)
              fuelingState = LIFTED_NOZZLE;
          break;
      case LIFTED_NOZZLE:                                   //The nozzle is lifted but the fueling hasn't started yet. 
          sprintf(line1, "Added cash: %d", cashSum);
          sprintf(line2, "Fuel price: %1.2f", fuelPrice);
          sendToLcd(line1, line2);
          GPIO_PORTF_DATA_R &= redLED;
          if (!lever)
          {
              xTimerStart(fuelPulse, portMAX_DELAY);    //Start works as a reset of the timer
              fuelingState = START_FUELING;
          }         
          break;
      case START_FUELING: //Already fueling but slower
          GPIO_PORTF_DATA_R &=  yellowLED;
          totalPrice = deliveredGas * fuelPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          break;
      case FUELING:                     // The timer will set the state when 2 seconds has passed.
          totalPrice = deliveredGas * fuelPrice;
          priceDifferece = cashSum - totalPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          GPIO_PORTF_DATA_R &= greenLED;

          if (lever || (priceDifferece < 10))
          {
              vTimerSetTimerID(fuelPulse, (void * ) 0);
              fuelingState = END_FUELING;
          }
          break;
      case END_FUELING: // Still fueling but slower
          totalPrice = deliveredGas * fuelPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          if (totalPrice < cashSum)
              vTimerSetTimerID(fuelPulse, (void*)0);
          GPIO_PORTF_DATA_R &= yellowLED;
      case FUELING_TERMINATE:
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          uartPrint("Terminated\r\n");
          xTimerStop(fuelPulse, pdMS_TO_TICKS(1000));
          //fuelingState = INIT;
          //setEVGroupPayment(localTaskEventGroup);
          break;

      default:
          break;
      }
      vTaskDelay(xBlockTime);



    }
  }
}


