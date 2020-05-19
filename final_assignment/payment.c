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
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_payment, pdFALSE, pdTRUE, portMAX_DELAY );
    if(uxBits == EV_GROUP_payment){


      //do stuff here
      uartPrint("fuel selection task's turn\r\n");
      vTaskDelay( xBlockTime );
      uartPrint("giving to next task in 1sec \r\n");
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
