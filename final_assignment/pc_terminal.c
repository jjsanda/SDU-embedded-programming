/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"

static SemaphoreHandle_t xSemaphorePc_terminal = NULL;
static INT16U pPc_terminalValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvPc_terminalTask( void *pvParameters );

/* getter and setters */
INT16U get_pc_terminal(){
  INT16U pc_terminalVal = -1;
  if( xSemaphoreTake( xSemaphorePc_terminal, portMAX_DELAY ) ){
    pc_terminalVal = pPc_terminalValue;
    xSemaphoreGive( xSemaphorePc_terminal );
  }
  return pc_terminalVal; //-1 if error, value else
}


/*-----------------------------------------------------------*/
BOOLEAN init_pc_terminal( void ){
  xSemaphorePc_terminal = xSemaphoreCreateMutex();
  if( xSemaphorePc_terminal != NULL ){
    xTaskCreate( prvPc_terminalTask, "pc_terminal task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    tilPrint("pc_terminal initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvPc_terminalTask( void *pvParameters )
{
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  for( ;; ){
    if( xSemaphoreTake( xSemaphorePc_terminal, 0 ) ){

       //work with pPc_terminalValue

      xSemaphoreGive( xSemaphorePc_terminal );
    }
    vTaskDelay( xBlockTime );
  }
}
