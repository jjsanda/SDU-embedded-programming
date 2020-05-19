/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
    INT8U valid = 0;
    int temp = 0;

    char output[] = "0000";
    INT8U digiRotation = 'A';
  const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_payment, pdFALSE, pdTRUE, portMAX_DELAY );
    if (uxBits == EV_GROUP_payment) {
       
        if (!CARD && !CASH)
        {
            move_LCD(0, 0);
            wr_str_LCD("Press * for card");
            move_LCD(0, 1);
            wr_str_LCD("Press # for cash");
        }
        
        key = waitForNextKey();
        if (key == '*')
        {
            
            move_LCD(0, 0);
            wr_str_LCD("Enter card number");
            move_LCD(0, 1);
            wr_str_LCD("                   ");

            CARD = 1;
        }
            
        if (key == '#')
            CASH = 1;

        if ((key != 0) && (key != '*') && CARD )
        {
            
            
            count++;
            switch (count)
            {
            case 1 ... 7:
                move_LCD(0, 0);
                wr_str_LCD("Enter card number");
                move_LCD(count - 1, 1);
                wr_ch_LCD(key);
                cardNr += key;
                break;
            case 8:
                cardNr += key;
                temp = cardNr % 2;
                move_LCD(count - 1, 1);
                wr_ch_LCD(key);


                move_LCD(0, 0);
                wr_str_LCD("Enter pin number");
                move_LCD(count, 1);
                wr_ch_LCD(' ');
                break;
            case 9 ... 11:
                pinNr += key;
                move_LCD(count, 1);
                wr_ch_LCD(key);
                break;
            case 12: 
                pinNr += key;
                valid = pinNr % 2;
                valid = valid - temp;  
                move_LCD(count, 1);
                wr_ch_LCD(key);
            default:
                break;
            }

            if (valid == 1)
            {
                move_LCD(0, 0);
                wr_str_LCD("Card and pin    ");
                move_LCD(0, 1);
                wr_str_LCD("is korrect        ");
            }
        }
        

         
        //-------------------------------------------------------------------------------
        // Cash payment 

        if ((key != 0) && (key != '#') && CASH)
        {
            digiRotation = getDigiRotation(); // get the value from the digiswitch. 

            output[0] = (digiRotation / 1000) + '0';               // gets most significant digit
            output[1] = ((digiRotation % 1000) / 100) + '0';
            output[2] = ((digiRotation % 100) / 10) + '0';
            output[3] = ((digiRotation % 10) / 1) + '0';

            uartPrint("digirotation er: ");
            uartPrint(output);
            uartPrint("\r\n");
        }
  
      //do stuff here
      //uartPrint("fuel selection task's turn\r\n");
      vTaskDelay( xBlockTime );
      //uartPrint("giving to next task in 1sec \r\n");
      vTaskDelay( xBlockTime );



//
//      if( xSemaphoreTake( xSemaphorePayment, 0 ) ){
//
//         //work with pPaymentValue
//
//        xSemaphoreGive( xSemaphorePayment );
//      }

      //if done - give to next task:
      uxBits = xEventGroupClearBits( localTaskEventGroup, EV_GROUP_payment ); // clear current bits first
      if(uxBits != EV_GROUP_payment){
        uartPrint("ERROR: clear of EV_GROUP_payment was not successful\r\n");
      }
      uxBits = xEventGroupSetBits( localTaskEventGroup, EV_GROUP_fuelsel ); // set bits for next task to be unblocked
      if(uxBits != EV_GROUP_fuelsel){
        uartPrint("ERROR: set of EV_GROUP_fuelsel was not successful\r\n");
      }
    }
  }
}
