/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "print.h"
#include "adc.h"
#include "adc.c"

static SemaphoreHandle_t xSemaphoreAI = NULL;
static INT16U pAdcValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvAiRead( void *pvParameters );

INT16U get_ai(){
  INT16U adcval = -1;
  if( xSemaphoreTake( xSemaphoreAI, portMAX_DELAY ) ){
    adcval = pAdcValue;
    xSemaphoreGive( xSemaphoreAI );
  }
  return adcval;
}

/*-----------------------------------------------------------*/
BOOLEAN init_ai( void ){
  init_adc();
  xSemaphoreAI = xSemaphoreCreateMutex();
  if( xSemaphoreAI != NULL ){
    xTaskCreate( prvAiRead, "AI read", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    tilPrint("ai initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvAiRead( void *pvParameters )
{
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  for( ;; ){
    if( xSemaphoreTake( xSemaphoreAI, 0 ) ){
      pAdcValue = get_adc();
      xSemaphoreGive( xSemaphoreAI );
    }
    vTaskDelay( xBlockTime );
  }
}
