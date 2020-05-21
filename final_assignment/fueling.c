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



//static SemaphoreHandle_t xSemaphoreFueling = NULL;
//static INT16U pFuelingValue = 0;
static float timerCallBackTimeSekunds = 1.0;
static TimerHandle_t fuelPulse = NULL;
static int fuelingState = INIT;
static float deliveredGas = 0;

//static TimerHandle_t fuelPulseTimer = NULL;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvFuelingTask( void *pvParameters );

///* getter and setters */
//INT16U get_fueling(){
//  INT16U fuelingVal = -1;
//  if( xSemaphoreTake( xSemaphoreFueling, portMAX_DELAY ) ){
//    fuelingVal = pFuelingValue;
//    xSemaphoreGive( xSemaphoreFueling );
//  }
//  return fuelingVal; //-1 if error, value else
//}


static int pulses = 0;
static int tempPulses = 0;
void callBackFunctionPulses(TimerHandle_t timer){
  pulses++;
}


//void callBackFunction(TimerHandle_t localFuelPulse)
//{
//    int callBackCount;
//
//    callBackCount = (int)pvTimerGetTimerID(fuelPulse);
//    callBackCount++;
//
//
//    switch (fuelingState)
//    {
//    case START_FUELING:
//        deliveredGas += 0.05 / timerCallBackTimeSekunds;
//        if (callBackCount >= (2 / timerCallBackTimeSekunds))
//            fuelingState = FUELING;
//        else
//            vTimerSetTimerID(fuelPulse, (void*)callBackCount);
//        break;
//    case FUELING:
//        deliveredGas += 0.3 / timerCallBackTimeSekunds;
//        break;
//    case END_FUELING:
//        deliveredGas += 0.05 / timerCallBackTimeSekunds;
//        if (callBackCount >= (5 / timerCallBackTimeSekunds))
//        {
//            fuelingState = FUELING_TERMINATE;
//            uartPrint("Fueling has been terminated\r\n");
//        }
//        else
//        {
//            vTimerSetTimerID(fuelPulse, (void*)callBackCount);
//            uartPrint("ved at v�re slut med fueling\r\n");
//        }
//        break;
//    default:
//        break;
//    }
//}

//const float slowPulseDelivery = 25.6; //512*0.05
//const float constantPulseDelivery = 153.6; //512*0.3

static const double slowPulseSpeed = ( ( 1 / (512*0.05) ) * 1000 );
static const double fastPulseSpeed = ( ( 1 / (512*0.3 ) ) * 1000 );

/*-----------------------------------------------------------*/
BOOLEAN init_fueling( void ){
  //fuelPulse = xTimerCreate("Timer", pdMS_TO_TICKS(1000 * timerCallBackTimeSekunds ), pdTRUE, (void*) 0, callBackFunction) ;
  fuelPulse = xTimerCreate("PulseTimer", pdMS_TO_TICKS(slowPulseSpeed), pdTRUE, (void*) 0, callBackFunctionPulses);
  if( fuelPulse != NULL ){
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
    char line1[17]; //16chars + terminating null
    char line2[17]; //16chars + terminating null
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
          pulses = 0;
          tempPulses = 0;
          sendToLcd(" Lift nozzle to ", " start fueling");
          break;
      case IDLE:
          if (!nozzleBoot){
             while(!nozzleBoot){
               vTaskDelay(pdMS_TO_TICKS(10));
             }
             vTaskDelay(pdMS_TO_TICKS(30)); //debaunce sw1
             fuelingState = LIFTED_NOZZLE;
          }
          break;
      case LIFTED_NOZZLE:                                   //The nozzle is lifted but the fueling hasn't started yet. 
          sprintf(line1, "Cash: %4i", cashSum);
          sprintf(line2, "Fuel price: %2.2f", fuelPrice);
          sendToLcd(line1, line2);
          GPIO_PORTF_DATA_R &= redLED;
          if (!lever)
          {
              //stop timeout timer
              xTimerStart(fuelPulse, portMAX_DELAY);    //Start works as a reset of the timer
              fuelingState = START_FUELING;
          }
          if(!nozzleBoot){
            fuelingState = FUELING_TERMINATE;
          }
          break;
      case START_FUELING: //Already fueling but slower
          GPIO_PORTF_DATA_R &=  yellowLED;
          deliveredGas = pulses * (1.0/512.0);
          totalPrice = deliveredGas * fuelPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          if(deliveredGas >= 0.1){ //after 2sec = 2*0.05l
            fuelingState = FUELING;
            xTimerChangePeriod( fuelPulse, pdMS_TO_TICKS(fastPulseSpeed), portMAX_DELAY);
          }
          //vTaskDelay( pdMS_TO_TICKS(slowPulseSpeed) ); // TODO: check if this will immprove lcd load
          break;
      case FUELING:                     // The timer will set the state when 2 seconds has passed.
          deliveredGas = pulses * (1.0/512.0);
          totalPrice = deliveredGas * fuelPrice;
          priceDifferece = cashSum - totalPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          GPIO_PORTF_DATA_R &= greenLED;


          if(priceDifferece < 10){ //TODO calculate the exact value here with the current fuel price
              xTimerChangePeriod( fuelPulse, pdMS_TO_TICKS(slowPulseSpeed), portMAX_DELAY);
              fuelingState = END_FUELING;
          }
          else if (lever) //if lever is released
          {
              xTimerChangePeriod( fuelPulse, pdMS_TO_TICKS(slowPulseSpeed), portMAX_DELAY);
              fuelingState = END_FUELING_RETURN_TO_LIFTED_NOZZLE;
              tempPulses = pulses;
          }
          break;
      case END_FUELING_RETURN_TO_LIFTED_NOZZLE: // Still fueling but slower

          deliveredGas = pulses * (1.0/512.0); //TODO: use function for that
          totalPrice = deliveredGas * fuelPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);

          if ((pulses - tempPulses) >= 25){ //25 pulses = 1sec in slow speed
              fuelingState = LIFTED_NOZZLE;
              //TODO: and we reset and start the timeout timer which will change the state to FUELING_TERMINATE if 15sec are counted down;
          }
          GPIO_PORTF_DATA_R &= yellowLED;
          break;
      case END_FUELING: // Still fueling but slower
          deliveredGas = pulses * (1.0/512.0);
          totalPrice = deliveredGas * fuelPrice;
          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);
          if (totalPrice >= (float) cashSum){
              fuelingState = FUELING_TERMINATE;
          }
          GPIO_PORTF_DATA_R &= yellowLED;
          break;
      case FUELING_TERMINATE:

          xTimerStop(fuelPulse, portMAX_DELAY); //STOPS PUMP (timer simulates the pump)
          deliveredGas = pulses * (1.0/512.0);
          totalPrice = deliveredGas * fuelPrice;

          sprintf(line1, "Liter: %4.2f", deliveredGas);
          sprintf(line2, "%1.2f %5.2f", fuelPrice, totalPrice);
          sendToLcd(line1, line2);

          uartPrint("Terminated\r\n");
          if(paymentType == CARD_PAYMENT_TYPE){
            appendLogEntry(10, fuelType, 100, totalPrice, "12341234"); //TODO: add card support, add day time, add operating time
          } else {
            appendLogEntry(10, fuelType, 100, totalPrice, "CASH"); //TODO: add card support, add day time, add operating time
          }
          fuelingState = INIT;
          setEVGroupPayment(localTaskEventGroup);
          break;

      default:
          break;
      }
      vTaskDelay(xBlockTime);



    }
  }
}


