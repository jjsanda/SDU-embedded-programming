/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "digi.h"


static QueueHandle_t xQueueDigi = NULL;
static INT16U pDigiValue = 0;
INT8U AHIGH = 0;
INT8U BHIGH = 0;
INT8U countRight = 0;
INT8U countLeft = 0;
INT8U AB = 0x00;
INT8U lastAB = 0x00;
INT8U lastState = 0;
INT16U value = 0;
/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvDigiTask( void *pvParameters );

/* getter and setters */
INT8U getDigiRotation(){
    INT8U data;
    if (xQueueReceive(xQueueDigi, &data, 0))
        return data;
    else
        return 0; 
}


/*-----------------------------------------------------------*/
BOOLEAN init_digi( void ){
    unsigned char initValueToSend = 'B';
  xQueueDigi = xQueueCreate( 1, sizeof( INT8U ) ); //l = left turn, r=right turn
  if( xQueueDigi != NULL ){
    xTaskCreate( prvDigiTask, "digi task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
    uartPrint("digi initialized\r\n");
    xQueueSend(xQueueDigi, &initValueToSend, 0U);
    return 1;
  } else {
    return 0;
  }
}



static void prvDigiTask( void *pvParameters )
{
    unsigned char ucValueToSend;
//    INT16U value = 0;
    INT16U lastValue = 0;
//    INT8U countRight = 0;
//    INT8U countLeft = 0;
//    INT8U AB = 0x00;
//    INT8U lastAB = 0x00;
//    INT8U lastState = 0;
    INT8U count = 0;
    INT8U valid_test;
    char output[] = "0000";

    //  Right + 100 
    //  Left + 10
    while (1) {
        count++;
        AB = (inputA | inputB);
        if (AB != lastAB) {
            if (lastState == 0) {               // hvis vi starter lavt 
                if (AB == 0x20) {                 // hvis A bliver høj    //0b0110 0000 0x20 0x40
                    value += 100;
                    countRight++;               //CW
                }
                else if (AB == 0x40) {            // hvis B bliver høj først
                    value += 10;
                    countLeft++;                //CCW
                }
            }
            else if (lastState == 1) {
                if (AB == 0x20) {                // hvis B bliver lav først
                    value += 10;
                    countLeft++;                //CCW
                }
                else if (AB == 0x40) {            // hvis A bliver lav først
                    value += 100;
                    countRight++;               //CW
                }
            }
        }
        
        if (AB)
            lastState = 1;
        else
            lastState = 0; 

        lastAB = AB;
        
        xQueueOverwrite(xQueueDigi, &value);

        output[0] = (value / 1000) + '0';               // gets most significant digit
        output[1] = ((value % 1000) / 100) + '0';
        output[2] = ((value % 100) / 10) + '0';
        output[3] = ((value % 10) / 1) + '0';

        if (lastValue != value)
        {
            uartPrint(" digiswitch value is: ");
            uartPrint(output);   
            uartPrint("\r\n");
            lastValue = value;
        }

        //move_LCD( 0, 0 );
        //wr_str_LCD("Trn 2 enter price");
        //move_LCD( 0, 1 );
        //wr_str_LCD(output);


        vTaskDelay(pdMS_TO_TICKS(1));

        //------------------------------

//        //vTaskDelay(10);
//        AB = (inputA | inputB);
//
//
//
//
//        valid_test = (AB >> 5) | lastAB;
//
//        switch (valid_test)
//        {
//        case 0:
//            break;
//        case 1:
//            countRight++;
//            value++;
//            ucValueToSend = 'r';
//            break;
//        case 2:
//            countLeft++;
//            value--;
//            ucValueToSend = 'l';
//            break;
//        case 4:
////            countLeft++;
//            ucValueToSend = 'l';
//            break;
//        case 7:
////            countRight++;
//            ucValueToSend = 'r';
//            break;
//        case 8:
////            countRight++;
//            ucValueToSend = 'r';
//            break;
//        case 11:
////            countLeft++;
//            ucValueToSend = 'l';
//            break;
//        case 14:
//            countRight++;
//            value++;
//            ucValueToSend = 'r';
//            break;
//        case 13:
//            countLeft++;
//            value--;
//            ucValueToSend = 'l';
//            break;
//        default: break;
//        }
//
//        lastAB = AB >> 3;
//        //ucValueToSend = value;
//
//        //xQueueSend(xQueueDigi, &ucValueToSend, 0U);




    }
}
