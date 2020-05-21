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
#include "payment.h"
#include "timers.h"

static SemaphoreHandle_t xSemaphorePayment = NULL;
static int finalCashSum = 0;
static char finalCardNum[9];
static int finalPaymentType = NO_PAYMENT_TYPE;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvPaymentTask( void *pvParameters );

int getCashSum(){
  return finalCashSum;
}
void getCardNum(char * buf){
    strcpy(buf,finalCardNum);
}
int getPaymentType(){
  return finalPaymentType;
}

/*-----------------------------------------------------------*/
BOOLEAN init_payment( void ){


  xSemaphorePayment = xSemaphoreCreateMutex();
  if( xSemaphorePayment != NULL ){
    if(xTaskCreate( prvPaymentTask, "payment task", 150, NULL, ( tskIDLE_PRIORITY + 3 ), NULL ) == pdPASS){
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
  //INT8U pinNr = 0;
  BOOLEAN cardOdd = 0;
  char cardNr[9];
  char pinNr[5];
  memset(finalCardNum,'-',8);
  memset(cardNr,'-',8);
  memset(pinNr,'-',4);
  cardNr[8] = '\0'; //make sure to zero terminate the strings! so sprintf will work
  cardNr[4] = '\0';
  INT8U count = 0;
  //INT8U cardMultiplier = 100000000;
  INT8U validCard = 0;
  char outputLCD[] = "0000";
  INT16U cashSum = 0;
  //char line1[17];
  char line2[32];
  //const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_payment, pdFALSE, pdTRUE, portMAX_DELAY );
    if (uxBits == EV_GROUP_payment) {
      finalCashSum = 0;
      finalPaymentType = NO_PAYMENT_TYPE;
       
        if (!CARD && !CASH){
          sendToLcd("Press * for card", "Press # for cash");

          //make sure to reset everything
          key = 0;
          memset(finalCardNum,'-',8);
          memset(cardNr,'-',8);
          memset(pinNr,'-',4);
          finalCardNum[8] = '\0';
          cardNr[8] = '\0'; //make sure to zero terminate the strings! so sprintf will work
          pinNr[4] = '\0';
          count = 0;
          validCard = 0;
          cashSum = 0;
          key = waitForNextKey(portMAX_DELAY);
        } else if(CASH){
          key = waitForNextKey( 0 ); //so we can quickly get the digi rotation
        } else { //CARD
          key = waitForNextKey(portMAX_DELAY); //since we only use keyboard
        }
//        key = waitForNextKey( 0 ); //so we can quickly get the digi rotation
        
        if (key == '*' && !CARD && !CASH)
        {
            sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
            sendToLcd("Enter card num.",line2);
            CARD = 1;
        }
            

        //if ((key != 0) && (key != '*') && CARD )
        if((key >= '0' && key <= '9') && CARD) //better check if its a number
        {
            
            switch (count)
            {
            case 0 ... 6:
                cardNr[count] = key;
                sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
                sendToLcd("Enter card num.",line2);
                break;
            case 7:
                cardNr[count] = key;
                cardOdd = (key - '0') % 2; //if we substract the ascii position of the zero we will end up with the actual int for that digit char
                sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
                sendToLcd("Enter pin number",line2);
                break;
            case 8 ... 10:
                //cardNr[count-8] = key;
                pinNr[count-8] = '*'; // keep away the spies :)
                sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
                sendToLcd("Enter pin number",line2);
                break;
            case 11: 
                pinNr[count-8] = '*';
                sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
                sendToLcd("Enter pin number",line2);
                vTaskDelay(pdMS_TO_TICKS(330));
                sendToLcd("processing.",line2);
                vTaskDelay(pdMS_TO_TICKS(330));
                sendToLcd("processing..",line2);
                vTaskDelay(pdMS_TO_TICKS(330));
                sendToLcd("processing...",line2);
                vTaskDelay(pdMS_TO_TICKS(330));
                BOOLEAN pinOdd = (key - '0') % 2;
                if((cardOdd && !pinOdd) || (!cardOdd && pinOdd)){
                  sendToLcd("pin correct",line2);
                  vTaskDelay(pdMS_TO_TICKS(1000));
                  sendToLcd("# to continue",line2);
                  validCard = 1;
                } else { //rejected
                  memset(pinNr,'-',4);
                  count = 7; //cause of count++
                  sprintf(line2,"%8s  %4s  ",cardNr,pinNr);
                  sendToLcd("wrong pin!",line2);
                  vTaskDelay(pdMS_TO_TICKS(1500));
                  sendToLcd("try again",line2);
                  vTaskDelay(pdMS_TO_TICKS(1000));
                  sendToLcd("Enter pin number",line2);
                }
                break;
            default:
                break;
            }
            count++;
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
                sendToLcd("# to submit cash", outputLCD);
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


      if ( (CASH || (validCard && CARD)) && (key == '#' ) ) {
          if(CASH) {
            finalPaymentType = CASH_PAYMENT_TYPE;
            finalCashSum = cashSum;
          }
          if(CARD) {
            finalPaymentType = CARD_PAYMENT_TYPE;
            strcpy(finalCardNum, cardNr);
            finalCardNum[8] = '\0';
          }

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
