/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"

static QueueHandle_t xQueueDigi = NULL;
static INT16U pDigiValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvDigiTask( void *pvParameters );

/* getter and setters */
int getDigiRotation(unsigned char * digiBuf, TickType_t xTicksToWait){
  return xQueueReceive( xQueuePrintTX, &digiBuf, xTicksToWait );
}


/*-----------------------------------------------------------*/
BOOLEAN init_digi( void ){
  xQueueDigi = xQueueCreate( 1024, sizeof( unsigned char ) ); //l = left turn, r=right turn
  if( xQueueDigi != NULL ){
    xTaskCreate( prvDigiTask, "digi task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    tilPrint("digi initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvDigiTask( void *pvParameters )
{
  unsigned char ucValueToSend;
  for( ;; ){
    //get digi rotation (PSEUDOCODE)
    //ucValueToSend = digi_getRotation(); //l = left turn, r=right turn
    // then send it to queue
    //xQueueSend( xQueuePrintRX, &ucValueToSend, 0U );
  }
}
