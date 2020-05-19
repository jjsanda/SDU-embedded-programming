/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "uart.h"

static QueueHandle_t xQueuePrintRX = NULL;
static QueueHandle_t xQueuePrintTX = NULL;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvRxTask( void *pvParameters );
static void prvTxTask( void *pvParameters );

void getNextLine(unsigned char * stringLineBuf, int maxLineLength, TickType_t xTicksToWaitForChar){
  unsigned char ucReceivedValue;
  int i = 0;
  //pseudocode
  //do{
  //  xQueueReceive( xQueuePrintRX, &ucReceivedValue, xTicksToWait );
  //  stringLineBuf[i++] = ucReceivedValue;
  //} while(ucReceivedValue != '\r' && i < maxLineLength)
  stringLineBuf[i++] = '\0';
  return 1;
}
//xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS
void uartGetChar(unsigned char * charBuf, TickType_t xTicksToWaitForChar){
  return xQueueReceive( xQueuePrintRX, charBuf, xTicksToWaitForChar );
}
//xSerialPutChar( xPort, cRxedChar, portMAX_DELAY );
void uartPutChar(unsigned char sendChar, TickType_t xTicksToWaitForChar){
  return xQueueSend( xQueuePrintTX, &sendChar, xTicksToWaitForChar );
}
void uartStrcpy(char* target, char* source1){
  while(*source1!='\0'){
    *(target++) = *(source1++);
  }
  *(target)='\0';
}
void uartConcat(char* target, char* source1, char* source2){
  while(*source1!='\0'){
    *(target++) = *(source1++);
    //target++;
  }
  while(*source2!='\0'){
    *(target++) = *(source2++);
    //target++;
  }
  *(target)='\0';
}

void uartPrint(char *string){
  while(*string!='\0') xQueueSend( xQueuePrintTX, (unsigned char *) string++, 0U );
}
void uartPrintDec(char * buf, int val, INT8U size){
  int weight = 1, i=0;
  while(size-- > 1) weight *= 10;
  unsigned char digit = 11;
  while(weight > 0){
    digit = val / weight;
    val %= weight;
    weight /= 10;
    if(digit >=0 && digit <= 9){ //check if it is a valid number, otherwise print 'x'
      digit += 48; //correct ascii mapping
    } else {
      digit = 'x';
    }
    buf[i++]=digit;
  }
  buf[i]='\0';
}
void uartPrint16U(INT16U val, INT8U size){
  char buf[size+1];
  uartPrintDec(buf,val,size);
  uartPrint(buf);
}
void uartBlockingPrint(char *string){
  while(*string!='\0') xQueueSend( xQueuePrintTX, (unsigned char *) string++, 10U );
}
/*-----------------------------------------------------------*/
BOOLEAN init_print( void ){
  uart0_init( 9600, 8, 1, 'n' );
  xQueuePrintRX = xQueueCreate( 1024, sizeof( unsigned char ) );
  xQueuePrintTX = xQueueCreate( 1024, sizeof( unsigned char ) );
  if( xQueuePrintRX != NULL && xQueuePrintTX != NULL ){
    xTaskCreate( prvRxTask, "print-RX", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    xTaskCreate( prvTxTask, "print-TX", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 4 ), NULL );
    uartPrint("\r\nprint initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvRxTask( void *pvParameters )
{
  unsigned char ucValueToSend;
  for( ;; )
  {
    if( uart0_rx_rdy() ){
      ucValueToSend = uart0_getc();
      xQueueSend( xQueuePrintRX, &ucValueToSend, 0U );
    } else {
      vTaskDelay( 1 );
    }
  }
}
/*-----------------------------------------------------------*/

static void prvTxTask( void *pvParameters )
{
  unsigned char ucReceivedValue;
  for( ;; )
  {
    if( uart0_tx_rdy() ){
      xQueueReceive( xQueuePrintTX, &ucReceivedValue, portMAX_DELAY );
      uart0_putc(ucReceivedValue);
      //vTaskDelay( 1 );
    } else {
      //vTaskDelay( 1 );
    }
  }
}
/*-----------------------------------------------------------*/
