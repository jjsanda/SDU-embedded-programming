/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "main.h"

static SemaphoreHandle_t xSemaphoreFueling = NULL;
static INT16U pFuelingValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvFuelingTask( void *pvParameters );

/* getter and setters */
INT16U get_fueling(){
  INT16U fuelingVal = -1;
  if( xSemaphoreTake( xSemaphoreFueling, portMAX_DELAY ) ){
    fuelingVal = pFuelingValue;
    xSemaphoreGive( xSemaphoreFueling );
  }
  return fuelingVal; //-1 if error, value else
}


/*-----------------------------------------------------------*/
BOOLEAN init_fueling( void ){
  xSemaphoreFueling = xSemaphoreCreateMutex();
  if( xSemaphoreFueling != NULL ){
    xTaskCreate( prvFuelingTask, "fueling task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    uartPrint("fueling initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvFuelingTask( void *pvParameters )
{
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_fueling, pdFALSE, pdTRUE, portMAX_DELAY );
    if(uxBits == EV_GROUP_fueling){


      //do stuff here
      //uartPrint("fueling task's turn\r\n");
      //vTaskDelay( xBlockTime );
      //uartPrint("giving to next task in 1sec \r\n");
      vTaskDelay( xBlockTime );


      //    if( xSemaphoreTake( xSemaphoreFueling, 0 ) ){
      //
      //       //work with pFuelingValue
      //
      //      xSemaphoreGive( xSemaphoreFueling );
      //    }
      //    vTaskDelay( xBlockTime );


      //if done - give to next task:
      uxBits = xEventGroupClearBits( localTaskEventGroup, EV_GROUP_fueling ); // clear current bits first
      if(uxBits != EV_GROUP_fueling){
        uartPrint("ERROR: clear of EV_GROUP_fueling was not successful\r\n");
      }
      uxBits = xEventGroupSetBits( localTaskEventGroup, EV_GROUP_payment ); // set bits for next task to be unblocked
      if(uxBits != EV_GROUP_payment){
        uartPrint("ERROR: set of EV_GROUP_payment was not successful\r\n");
      }
    }
  }
}


/*-----------------------------------------------------------*/
/*
 * EXAMPLE Code for freeRTOS state machine. I guess fueling task should be done in a simmilar manner
 *
#define IDLE          0
#define MENU_OPTIONS  1
#define EDIT_OFFSET   2
#define EDIT_SCALE    3

static void prvUiTask( void *pvParameters )
{
  unsigned char ucReceivedValue;
  char ch2str[2];
  INT8U state = 0;

  INT16U local_offset = 0;
  char offset_buf[9];
  INT16U local_scale = 0;
  char scale_buf[9];
  INT8U digit = 100;

  for( ;; )
  {
    // press any key to enter in menu mode:
    xQueueReceive( xQueueKeyboard, &ucReceivedValue, portMAX_DELAY );

    //TIL: print key for debugging
    uartPrint("\r\nkey: ");
    ch2str[0]=ucReceivedValue; ch2str[1]='\0';
    uartPrint(ch2str);
    uartPrint("\r\n");

    switch(state) {
      case IDLE:
        lcd_send("Options - exit:*",
                 "offset:1 scale:2");
        state = MENU_OPTIONS;
        lcd_take();
        break;
      case MENU_OPTIONS:
        if(ucReceivedValue=='1'){
          lcd_send("type in offset",
                   "submit with #");
          state = EDIT_OFFSET;
        } else if(ucReceivedValue=='2'){
          lcd_send("type in scale",
                   "submit with #");
          state = EDIT_SCALE;
        } else if(ucReceivedValue=='*'){
          lcd_send("exit","");
          state = IDLE;
          lcd_give();
        } else {
          lcd_send("Options - 1: offset",
                   "2: scale, *: exit");
        }
        break;
      case EDIT_OFFSET:
        digit = ucReceivedValue - 48; //48 is ascii offset
        if(digit >= 0 && digit <= 9){
          local_offset *= 10;
          local_offset += digit;
          local_offset %= 10000; //make sure it only uses 5 digits
          uartPrintDec(offset_buf, local_offset, 4);
          lcd_send("new offset:", offset_buf);
        } else if(ucReceivedValue=='#'){
          set_offset(local_offset);
          uartPrintDec(offset_buf, local_offset, 8);
          lcd_send("saved offset:", offset_buf);
          state = IDLE;
          lcd_give();
        } else if(ucReceivedValue=='*'){
          lcd_send("exit",
                   "aborted change");
          local_offset = 0;
          state = IDLE;
          lcd_give();
        }
        break;
      case EDIT_SCALE:
        digit = ucReceivedValue - 48; //48 is ascii offset
        if(digit >= 0 && digit <= 9){
          local_scale *= 10;
          local_scale += digit;
          local_scale %= 10000; //make sure it only uses 5 digits
          uartPrintDec(scale_buf, local_scale, 4);
          lcd_send("new: scale / 100:", scale_buf);
        } else if(ucReceivedValue=='#'){
          float factor = 1;
          factor = (float) local_scale / 100;
          set_scalefactor(factor);
          uartPrintDec(scale_buf, local_scale, 8);
          lcd_send("saved scale:", scale_buf);
          state = IDLE;
          lcd_give();
        } else if(ucReceivedValue=='*'){
          lcd_send("exit",
                   "aborted change");
          local_offset = 0;
          state = IDLE;
          lcd_give();
        }
        break;

    }
    ucReceivedValue=0;
  }
}
*/
/*-----------------------------------------------------------*/

