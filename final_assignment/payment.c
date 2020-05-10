/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "print.h"
#include "emp_type.h"

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
  for( ;; ){
    if( xSemaphoreTake( xSemaphorePayment, 0 ) ){

       //work with pPaymentValue

      xSemaphoreGive( xSemaphorePayment );
    }
    vTaskDelay( xBlockTime );
  }
}
