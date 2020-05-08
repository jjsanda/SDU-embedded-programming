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
#include "message_buffer.h"

#define LCD_LINE_LENGTH 16

static MessageBufferHandle_t xMessageBufferLCD = NULL;
static SemaphoreHandle_t xSemaphoreLCDSend = NULL;
static SemaphoreHandle_t xSemaphoreLCDOwner = NULL;

typedef struct displayPayload {
  char line1[LCD_LINE_LENGTH];
  char line2[LCD_LINE_LENGTH];
} displayPayload;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvLcdOut( void *pvParameters );

void lcd_take(){
  xSemaphoreTake( xSemaphoreLCDOwner, portMAX_DELAY );
}
void lcd_give(){
  xSemaphoreGive( xSemaphoreLCDOwner );
}

int lcd_send(char *line1, char *line2){
  const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
  displayPayload payload;
  //for(int i=0; i<LCD_LINE_LENGTH && line1[i]!='\0'; i++){
  for(int i=0; i<LCD_LINE_LENGTH; i++){
    payload.line1[i] = line1[i];
  }
  for(int i=0; i<LCD_LINE_LENGTH; i++){
    payload.line2[i] = line2[i];
  }
  size_t xBytesSent;

  // Obtain the semaphore - block 100ms if the semaphore is not available
  if( xSemaphoreTake( xSemaphoreLCDSend, 0 ) ){
      // We now have the semaphore and can access the shared resource.

      /* Send an array to the message buffer, blocking for a maximum of 100ms to
      wait for enough space to be available in the message buffer. */
      xBytesSent = xMessageBufferSend( xMessageBufferLCD,
                                       ( void * ) &payload,
                                       sizeof( payload ),
                                       xBlockTime );
      // We have finished accessing the shared resource so can free the
      // semaphore.
      if( xSemaphoreGive( xSemaphoreLCDSend ) != pdTRUE )
      {
          // We would not expect this call to fail because we must have
          // obtained the semaphore to get here.
          return 0; //send FAILED
      }
  } else {
    return 0; //send FAILED
  }

  if( xBytesSent != sizeof( payload ) ){
    return 0; //send FAILED
  } else {
    return 1; //send SUCCEEDED
  }

}

/*-----------------------------------------------------------*/
BOOLEAN init_lcd( void ){
  xMessageBufferLCD = xMessageBufferCreate( sizeof( displayPayload ) + sizeof( size_t ) ); //2 row display with 16 chars each
  xSemaphoreLCDSend = xSemaphoreCreateMutex();
  xSemaphoreLCDOwner = xSemaphoreCreateMutex();
  if( xMessageBufferLCD != NULL && xSemaphoreLCDSend != NULL && xSemaphoreLCDOwner != NULL){
    xTaskCreate( prvLcdOut, "LCD out", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    tilPrint("lcd initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvLcdOut( void *pvParameters )
{
  displayPayload ucRxData;
  size_t xReceivedBytes;
  for( ;; )
  {
    /* Receive the next message from the message buffer.  Wait in the Blocked
    state (so not using any CPU processing time) for portMAX_DELAY ticks for
    a message to become available. */
    xReceivedBytes = xMessageBufferReceive( xMessageBufferLCD,
                                            ( void * ) &ucRxData,
                                            sizeof( ucRxData ),
                                            portMAX_DELAY );

    if( xReceivedBytes > 0 )
    {
      /* A ucRxData contains a message that is xReceivedBytes long.  Process
      the message here.... */
      tilPrint("\r\n\r\nsimulate LCD screen:\r\n");
      tilPrint("----------------\r\n");
      char buf1[20];
      tilConcat(buf1,ucRxData.line1," "); //make sure we have a '\0' to terminate the string
      buf1[16]='\0';
      tilPrint(buf1);
      tilPrint("\r\n");
      char buf2[20];
      tilConcat(buf2,ucRxData.line2," "); //make sure we have a '\0' to terminate the string
      buf2[16]='\0';
      tilPrint(buf2);
      tilPrint("\r\n");
      tilPrint("----------------\r\n");
    }
  }
}
