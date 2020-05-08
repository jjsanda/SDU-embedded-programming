/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include <scale.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "print.h"

static QueueHandle_t xQueueKeyboard = NULL;

/*-----------------------------------------------------------*/
static void prvKeyboardTask( void *pvParameters );
static void prvUiTask( void *pvParameters );

/* --- public getter & setter functions ---*/
int waitForNextKey(unsigned char keyBuffer, TickType_t xTicksToWait){
  return xQueueReceive( xQueueKeyboard, &keyBuffer, xTicksToWait );
}

/*-----------------------------------------------------------*/
BOOLEAN init_keyboard( void ){
  xQueueKeyboard = xQueueCreate( 256, sizeof( unsigned char ) );
  if( xQueueKeyboard != NULL ){
    xTaskCreate( prvKeyboardTask, "Keyboard Task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
//    xTaskCreate( prvUiTask, "UI Task", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 4 ), NULL );
    tilPrint("UI & keyboard initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}


INT8U row( INT8U y ){
  INT8U result = 0;
  switch( y )
  {
    case 0x01: result = 1; break;
    case 0x02: result = 2; break;
    case 0x04: result = 3; break;
    case 0x08: result = 4; break;
  }
  return( result );
}

INT8U key_catch( INT8U x, INT8U y ){
  const INT8U matrix[3][4] = {{'*','7','4','1'},
                              {'0','8','5','2'},
                              {'#','9','6','3'}};
  return( matrix[x-1][y-1] );
}

BOOLEAN check_column(INT8U x){
    INT8U y = GPIO_PORTE_DATA_R & 0x0F;             // Save the values of the 4 bits for the rows
    if( y )                                         // If one of them are set...
    {                                               // ...we first find the row number with the function row()
        unsigned char ch = key_catch( x, row(y) );          // Now that we have the row and column we look up the corresponding character using the function key_catch
        xQueueSend( xQueueKeyboard, &ch, 0U );
        return 1;
    }
    return 0;
}

static void prvKeyboardTask( void *pvParameters )
{
  INT8U state = 0;
  for( ;; ){
    vTaskDelay( 10 ); //50ms to debounce
    switch(state) {
      case 0:
        GPIO_PORTA_DATA_R &= 0xE3;          // Clear the 3 bits for the columns
        GPIO_PORTA_DATA_R |= 0x10;          // Set the bit for column 1
        if (check_column(1))                // Check all the rows for column 1, using the function check_column
        {                                   // If a button press is registered we go to next state so the press is only registered once
            state = 1;
            break;
        }
        GPIO_PORTA_DATA_R &= 0xE3;          // Repeat the above for the two other columns
        GPIO_PORTA_DATA_R |= 0x08;
        if (check_column(2))
        {
            state = 1;
            break;
        }
        GPIO_PORTA_DATA_R &= 0xE3;
        GPIO_PORTA_DATA_R |= 0x04;
        if (check_column(3))
        {
            state = 1;
            break;
        }
        break;
      case 1:
        if( !(GPIO_PORTE_DATA_R & 0x0F) )   // We stay here until the button is released so a button press is not counted more than once
        {
          state = 0;
        }
        break;
    }
  }
}

#define IDLE          0
#define MENU_OPTIONS  1
#define EDIT_OFFSET   2
#define EDIT_SCALE    3


/*-----------------------------------------------------------*/
/*static void prvUiTask( void *pvParameters )
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
    tilPrint("\r\nkey: ");
    ch2str[0]=ucReceivedValue; ch2str[1]='\0';
    tilPrint(ch2str);
    tilPrint("\r\n");

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
          tilPrintDec(offset_buf, local_offset, 4);
          lcd_send("new offset:", offset_buf);
        } else if(ucReceivedValue=='#'){
          set_offset(local_offset);
          tilPrintDec(offset_buf, local_offset, 8);
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
          tilPrintDec(scale_buf, local_scale, 4);
          lcd_send("new: scale / 100:", scale_buf);
        } else if(ucReceivedValue=='#'){
          float factor = 1;
          factor = (float) local_scale / 100;
          set_scalefactor(factor);
          tilPrintDec(scale_buf, local_scale, 8);
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
