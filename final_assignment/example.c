/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"

static SemaphoreHandle_t xSemaphoreExample = NULL;
static INT16U pExampleValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvExampleTask( void *pvParameters );

/* getter and setters */
INT16U get_example(){
  INT16U exampleVal = -1;
  if( xSemaphoreTake( xSemaphoreExample, portMAX_DELAY ) ){
    exampleVal = pExampleValue;
    xSemaphoreGive( xSemaphoreExample );
  }
  return exampleVal; //-1 if error, value else
}


/*-----------------------------------------------------------*/
BOOLEAN init_example( void ){
  xSemaphoreExample = xSemaphoreCreateMutex();
  if( xSemaphoreExample != NULL ){
    xTaskCreate( prvExampleTask, "example task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    uartPrint("example initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvExampleTask( void *pvParameters )
{
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  for( ;; ){
    if( xSemaphoreTake( xSemaphoreExample, 0 ) ){

       //work with pExampleValue

      xSemaphoreGive( xSemaphoreExample );
    }
    vTaskDelay( xBlockTime );
  }
}
