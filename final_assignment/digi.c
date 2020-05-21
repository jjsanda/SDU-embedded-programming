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
#include "digi.h"
#include "main.h"


static QueueHandle_t xQueueDigi = NULL;
//static INT16U pDigiValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvDigiTask( void *pvParameters );

/* getter and setters */
int getDigiRotation(){
    INT16U data = 0;
    if (xQueueReceive(xQueueDigi, &data, 0) == pdTRUE)
        return data;
    else
        return 0;
}


/*-----------------------------------------------------------*/
BOOLEAN init_digi( void ){
  xQueueDigi = xQueueCreate( 100, sizeof( INT16U ) ); //l = left turn, r=right turn
  if( xQueueDigi != NULL ){
    xTaskCreate( prvDigiTask, "digi task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ) , NULL );
    uartPrint("digi initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}


static void prvDigiTask( void *pvParameters )
{

    EventBits_t uxBits;
    EventGroupHandle_t localTaskEventGroup = getEvGroup();
    INT16U ucValueToSend = 0;
    INT16U lastValue = 0;

    INT8U AB = 0x00;
    INT8U lastAB = 0x00;
    INT8U lastState = 0;
    char output[] = "0000";


    while (1) {
        AB = (inputA | inputB);
        if (AB != lastAB) {
            if (lastState == 0) {                       // If A and B is low to start with
                if (AB == 0x20) {                       // If A turns high first    //0b0110 0000 0x20 0x40
                    ucValueToSend = 100;               // Then it is a clock wise rotation
                    xQueueSend(xQueueDigi, &ucValueToSend, portMAX_DELAY);
                }
                else if (AB == 0x40) {                  // If B turns high first 
                    ucValueToSend = 10;                // Then it is a counter clock wise rotation
                    xQueueSend(xQueueDigi, &ucValueToSend, portMAX_DELAY);
                }
            }
            else if (lastState == 1) {                  // If A and B is high to start with
                if (AB == 0x20) {                       // and B turns low first
                    ucValueToSend = 10;                // then is a counter clock wise rotation
                    xQueueSend(xQueueDigi, &ucValueToSend, portMAX_DELAY);
                }
                else if (AB == 0x40) {                  // But if A turns low first
                    ucValueToSend = 100;               // Then it is a clock wise rotation
                    xQueueSend(xQueueDigi, &ucValueToSend, portMAX_DELAY);
                }
            }
        }
        if (AB)
            lastState = 1;
        else
            lastState = 0; 

        lastAB = AB;

        vTaskDelay(pdMS_TO_TICKS(5));
    }

    
}
