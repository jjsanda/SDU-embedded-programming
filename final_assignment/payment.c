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
#include "print.h"
#include "emp_type.h"
#include "main.h"
#include "digi.h"
#include "keyboard.h"
#include "lcd.h"

static SemaphoreHandle_t xSemaphorePayment = NULL;
static INT16U pPaymentValue = 0;


/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvPaymentTask( void *pvParameters );

/* getter and setters */
INT16U get_payment(){
  INT16U paymentVal = -1;
  if( xSemaphoreTake( xSemaphorePayment, portMAX_DELAY ) ){
    paymentVal = pPaymentValue;
    xSemaphoreGive( xSemaphorePayment );
  }
  return paymentVal; //-1 if error, value else
}


/*-----------------------------------------------------------*/
BOOLEAN init_payment( void ){

  xSemaphorePayment = xSemaphoreCreateMutex();
  if( xSemaphorePayment != NULL ){
    if(xTaskCreate( prvPaymentTask, "payment task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL ) == pdPASS){
      uartPrint("payment initialized\r\n");
      return 1;
    } else {
      uartPrint("ERROR: payment TASK NOT initialized\r\n");
    }
  } else {
    uartPrint("ERROR: payment SEM NOT initialized\r\n");
  }
  return 0;
}

static void prvPaymentTask( void *pvParameters )
{
    BOOLEAN CARD = 0;
    BOOLEAN CASH = 0;
    INT8U key = 0;
    INT8U pinNr = 0;
    INT16U cardNr = 0; 
    INT8U count = 0;
    INT8U validCard = 0;
    char outputLCD[] = "0000";
    INT16U cashSum = 0;
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_payment, pdFALSE, pdTRUE, portMAX_DELAY );
    if (uxBits == EV_GROUP_payment) {
       
        if (!CARD && !CASH)
            sendToLcd("Press * for card", "Press # for cash");
        
        key = waitForNextKey();
        if (key == '*')
        {
            sendToLcd("Enter card number","");
            CARD = 1;
        }
            

        if ((key != 0) && (key != '*') && CARD )
        {
            
            switch (count)
            {
            case 0 ... 6:
                move_LCD(0, 0);
                wr_str_LCD("Enter card number");
                move_LCD(count, 1);
                wr_ch_LCD(key);
                cardNr += key % 2;
                break;
            case 7:
                cardNr += key % 2;
                move_LCD(count, 1);
                wr_ch_LCD(key);

                move_LCD(0, 0);
                wr_str_LCD("Enter pin number");
                move_LCD(count + 1, 1);
                wr_ch_LCD(' ');
                break;
            case 8 ... 10:
                pinNr += key % 2;
                move_LCD(count, 1);
                wr_ch_LCD(key);
                break;
            case 11: 
                pinNr += key % 2;
                move_LCD(count, 1);
                wr_ch_LCD(key);
            default:
                break;
            }
            count++;
            // If the card and pin is valid the fuelselect task should run 
          
         
            if (cardNr == 8 && pinNr == 0 && count >= 12)                  //Odd card number and even pin number 
            {
                pinNr = 0;
                cardNr =0;
                count = 0;
                validCard = 1;
                send_LCD("Card and pin", " is korrect");

            }
            else if (cardNr == 0 && pinNr == 4 && count >= 12)             //Even card number and odd pin number
            {
                pinNr = 0;
                cardNr = 0;
                count = 0;
                validCard = 1;
                send_LCD("Card and pin", " is korrect");
            }
            else if (count >= 12 && validCard != 1)
            {

                pinNr = 0;
                cardNr = 0;
                count = 0;
                send_LCD("Card or pin", "not korrect");
            }
                        
        }
        
        //-------------------------------------------------------------------------------
        // Cash payment 

        if (key == '#' && !CASH && !CARD)
        {
            key = 0;
            sendToLcd("Turn digiswitch", "R = 100 & L = 10");;
            CASH = 1;
        }

        if (CASH)
        {
            int digiValue = getDigiRotation();
           //cashSum += digiValue // get the value from the digiswitch.
            if (digiValue)
            {
                cashSum += digiValue; // get the value from the digiswitch.
                outputLCD[0] = (cashSum / 1000) + '0';               // gets most significant digit
                outputLCD[1] = ((cashSum % 1000) / 100) + '0';
                outputLCD[2] = ((cashSum % 100) / 10) + '0';
                outputLCD[3] = ((cashSum % 10) / 1) + '0';
                send_LCD("Total value", outputLCD);
            }
        }
  
      //do stuff here
//      uartPrint("fuel selection task's turn\r\n");
//      vTaskDelay( xBlockTime );
//      uartPrint("giving to next task in 1sec \r\n");
//      vTaskDelay( xBlockTime );


//
//      if( xSemaphoreTake( xSemaphorePayment, 0 ) ){
//
//         //work with pPaymentValue
//
//        xSemaphoreGive( xSemaphorePayment );
//      }

      //if done - give to next task:
      if ( (CASH || validCard) && (key == '#' ) ) {
          CASH = 0;
          CARD = 0;
          key = 0;
          cashSum = 0;
          uxBits = xEventGroupClearBits(localTaskEventGroup, EV_GROUP_payment); // clear current bits first
          if (uxBits != EV_GROUP_payment) {
              uartPrint("ERROR: clear of EV_GROUP_payment was not successful\r\n");
          }
          uxBits = xEventGroupSetBits(localTaskEventGroup, EV_GROUP_fuelsel); // set bits for next task to be unblocked
          if (uxBits != EV_GROUP_fuelsel) {
              uartPrint("ERROR: set of EV_GROUP_fuelsel was not successful\r\n");
          }
      }
    }
  }
}
