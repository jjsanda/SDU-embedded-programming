/******************************************************************************
 *
 * NOTE 1:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * configCREATE_SIMPLE_TICKLESS_DEMO setting in FreeRTOSConfig.h is used to
 * select between the two.  See the notes on using
 * configCREATE_SIMPLE_TICKLESS_DEMO in main.c.  This file implements the
 * simply blinky style version.
 *
 * The blinky demo uses FreeRTOS's tickless idle mode to reduce power
 * consumption.  See the notes on the web page below regarding the difference
 * in power saving that can be achieved between using the generic tickless
 * implementation (as used by the blinky demo) and a tickless implementation
 * that is tailored specifically to the CC3220.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware, are defined in main.c.
 *
 * See http://www.FreeRTOS.org/TI_CC3220_SimpleLink_FreeRTOS_Demo.html for
 * instructions.
 *
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 200 milliseconds.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, toggles the LED.  The 'block
 * time' parameter passed to the queue receive function specifies that the
 * task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include <scale.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "string.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "print.h"
#include "lcd.h"
#include "ai.h"

static SemaphoreHandle_t xSemaphoreValue = NULL;

static void prvScaleTask( void *pvParameters );
static float scalefactor = 1.5;
static INT16U offset = 1000;

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
BOOLEAN init_scale( void ){
  xSemaphoreValue = xSemaphoreCreateMutex();
  if( xSemaphoreValue != NULL ){
    xTaskCreate( prvScaleTask, "AI read", 128, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    tilPrint("scale initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

void set_scalefactor(float val){
  if( xSemaphoreTake( xSemaphoreValue, portMAX_DELAY ) ){
    scalefactor = val;
    xSemaphoreGive( xSemaphoreValue );
  }
}

void set_offset(INT16U val){
  if( xSemaphoreTake( xSemaphoreValue, portMAX_DELAY ) ){
    offset = val;
    xSemaphoreGive( xSemaphoreValue );
  }
}

static void prvScaleTask( void *pvParameters )
{
  static const TickType_t x1sec= pdMS_TO_TICKS( 1000 );
  char dec_buf[9];
  char line2_buf[20];

  for( ;; )
  {
    INT16U ai = get_ai(); // we wait here as long as we have read a new adc value (synced with semaphore)
    INT32U val = 0;
    if( xSemaphoreTake( xSemaphoreValue, portMAX_DELAY ) ){
      val = ((INT32U) ai * scalefactor) + (INT32U) offset;
      xSemaphoreGive( xSemaphoreValue );
    }

    tilPrintDec(dec_buf, val, 8);
    tilConcat(line2_buf, "adcval: ", dec_buf);

    lcd_take();
    lcd_send(line2_buf,"");
    lcd_give();

    vTaskDelay( x1sec );
  }
}
/*-----------------------------------------------------------*/

