

/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "print.h"
#include "message_buffer.h"
#include "uart.h"
#include "lcd.h"


/*****************************    Defines    *******************************/



#define LCD_LINE_LENGTH 16
#define QUEUE_LEN   128
static SemaphoreHandle_t xSemaphoreLCDSend = NULL;
static QueueHandle_t Q_LCD;

typedef struct displayPayload {
  char line1[LCD_LINE_LENGTH];
  char line2[LCD_LINE_LENGTH];
} displayPayload;

enum LCD_states
{
  LCD_POWER_UP,
  LCD_INIT,
  LCD_READY,
  LCD_ESC_RECEIVED,
};

INT8U Q_space = 0;

/*****************************   Constants   *******************************/
const INT8U LCD_init_sequense[]=
{
  0x30,     // Reset
  0x30,     // Reset
  0x30,     // Reset
  0x20,     // Set 4bit interface
  0x28,     // 2 lines Display
  0x0C,     // Display ON, Cursor OFF, Blink OFF
  0x06,     // Cursor Increment
  0x01,     // Clear Display
  0x02,     // Home
  0xFF      // stop
};

/*****************************   Variables   *******************************/
//INT8U LCD_buf[QUEUE_LEN];
//INT8U LCD_buf_head = 0;
//INT8U LCD_buf_tail = 0;
//INT8U LCD_buf_len  = 0;

enum LCD_states LCD_state = LCD_POWER_UP;
INT8U LCD_init;



/*****************************   Functions   *******************************/
INT8U wr_ch_LCD( INT8U Ch )
/*****************************************************************************
*   OBSERVE  : LCD_PROC NEEDS 20 mS TO PRINT OUT ONE CHARACTER
*   Function : See module specification (.h-file).
*****************************************************************************/
{


  return (xQueueSendToBack( Q_LCD, &Ch, portMAX_DELAY));
}

void wr_str_LCD( INT8U *pStr )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{

  while( *pStr )
  {
    wr_ch_LCD( *pStr );
    pStr++;
  }
}

void move_LCD( INT8U x, INT8U y )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8U Pos;

  Pos = y*0x40 + x;
  Pos |= 0x80;
  wr_ch_LCD( ESC );
  wr_ch_LCD( Pos );
}
//----------------------------

void wr_ctrl_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write low part of control data to LCD.
******************************************************************************/
{
  INT8U temp;
  volatile int i;

  temp = GPIO_PORTC_DATA_R & 0x0F;
  temp  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTC_DATA_R  = temp;
  for( i=0; i<1000; i )
      i++;
  GPIO_PORTD_DATA_R &= 0xFB;        // Select Control mode, write
  for( i=0; i<1000; i )
      i++;
  GPIO_PORTD_DATA_R |= 0x08;        // Set E High

  for( i=0; i<1000; i )
      i++;

  GPIO_PORTD_DATA_R &= 0xF7;        // Set E Low

  for( i=0; i<1000; i )
      i++;
}

void wr_ctrl_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write high part of control data to LCD.
******************************************************************************/
{
  wr_ctrl_LCD_low(( Ch & 0xF0 ) >> 4 );
}

void out_LCD_low( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send low part of character to LCD.
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  INT8U temp;

  temp = GPIO_PORTC_DATA_R & 0x0F;
  GPIO_PORTC_DATA_R  = temp | ((Ch & 0x0F) << 4);
  //GPIO_PORTD_DATA_R &= 0x7F;        // Select write
  GPIO_PORTD_DATA_R |= 0x04;        // Select data mode
  GPIO_PORTD_DATA_R |= 0x08;        // Set E High
  GPIO_PORTD_DATA_R &= 0xF7;        // Set E Low
}

void out_LCD_high( INT8U Ch )
/*****************************************************************************
*   Input    : Mask
*   Output   : -
*   Function : Send high part of character to LCD.
*              This function works only in 4 bit data mode.
******************************************************************************/
{
  out_LCD_low((Ch & 0xF0) >> 4);
}

void wr_ctrl_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  static INT8U Mode4bit = 0;
  INT16U i;

  wr_ctrl_LCD_high( Ch );
  if( Mode4bit )
  {
    for(i=0; i<1000; i++);
    wr_ctrl_LCD_low( Ch );
  }
  else
  {
    if( (Ch & 0x30) == 0x20 )
      Mode4bit = 1;
  }
}

void clr_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Clear LCD.
******************************************************************************/
{
  //gfprintf(1, "%c%c                                                                                       ", 0x1B, 0x80);
  wr_ctrl_LCD( 0x01 );
}


void home_LCD()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Return cursor to the home position.
******************************************************************************/
{
  wr_ctrl_LCD( 0x02 );
}

void Set_cursor( INT8U Ch )
/*****************************************************************************
*   Input    : New Cursor position
*   Output   : -
*   Function : Place cursor at given position.
******************************************************************************/
{
  wr_ctrl_LCD( Ch );
}


void out_LCD( INT8U Ch )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write control data to LCD.
******************************************************************************/
{
  INT16U i;

  out_LCD_high( Ch );
  for(i=0; i<1000; i++);
  out_LCD_low( Ch );
}





BOOLEAN init_lcd( void )
{
  xSemaphoreLCDSend = xSemaphoreCreateMutex();
  Q_LCD = xQueueCreate(128, sizeof(INT8U));
  if( Q_LCD != NULL && xSemaphoreLCDSend != NULL ){
    xTaskCreate( lcd_task, "LCD out", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL );
      uartPrint("lcd initialized\r\n");
    return 1;
  } else {
    return 0;
  }
  return 0;
}


int sendToLcd(char* line1, char* line2)
{
    char firstLine[LCD_LINE_LENGTH];
    char secondLine[LCD_LINE_LENGTH];
    BOOLEAN lineEnd = 0;
    for (int i = 0; i < LCD_LINE_LENGTH; i++)
    {
        if (line1[i] == '\0')
            lineEnd = 1;
        if (lineEnd)
            firstLine[i] = ' ';
        else
            firstLine[i] = line1[i];
    }
    lineEnd = 0;
    for (int i = 0; i < LCD_LINE_LENGTH; i++)
    {
        if (line2[i] == '\0')
            lineEnd = 1;
        if (lineEnd)
            secondLine[i] = ' ';
        else
            secondLine[i] = line2[i];
    }
 
    move_LCD(0, 0);
    wr_str_LCD((unsigned char *) firstLine);
    move_LCD(0, 1);
    wr_str_LCD((unsigned char *) secondLine);

//    uartPrint("simulate LCD:\r\n");
//    uartPrint("----------------\r\n");
//    uartPrint(firstLine);
//    uartPrint("\r\n");
//    uartPrint(secondLine);
//    uartPrint("\r\n");
//    uartPrint("----------------\r\n");
}

/*
int sendToLcd(char* line1, char* line2) {
    const TickType_t xBlockTime = pdMS_TO_TICKS(100);
    displayPayload payload;
    //for(int i=0; i<LCD_LINE_LENGTH && line1[i]!='\0'; i++){
    BOOLEAN lineEnd = 0;
    char stuffing = ' ';

    // Obtain the semaphore - block 100ms if the semaphore is not available
    if (xSemaphoreTake(xSemaphoreLCDSend, xBlockTime)) {
        // We now have the semaphore and can access the shared resource.

        for (int i = 0; i < LCD_LINE_LENGTH; i++) {

           if (line1[i] == '\0')
                lineEnd = 1;
           if (lineEnd)
               xQueueSendToBack(Q_LCD, &stuffing, 0);
           else
            xQueueSendToBack(Q_LCD, &line1[i], 0);
           if (i == LCD_LINE_LENGTH - 1)
                move_LCD(0, 1);
        }

        lineEnd = 0;

        for (int i = 0; i < LCD_LINE_LENGTH; i++) {
            if (line1[i] == '\0')
                lineEnd = 1;
            if (lineEnd)
                xQueueSendToBack(Q_LCD, &stuffing, 0);
            else
                xQueueSendToBack(Q_LCD, &line2[i], 0);
            if (i == LCD_LINE_LENGTH - 1)
                move_LCD(0, 0);
        }



//        Send an array to the message buffer, blocking for a maximum of 100ms to
//        wait for enough space to be available in the message buffer.
//        xBytesSent = xMessageBufferSend(xMessageBufferLCD,
//            (void*)&payload,
//            sizeof(payload),
//            xBlockTime);
//


        // We have finished accessing the shared resource so can free the
        // semaphore.
        if (xSemaphoreGive(xSemaphoreLCDSend) != pdTRUE)
        {
            // We would not expect this call to fail because we must have
            // obtained the semaphore to get here.
            return 0; //send FAILED
        }
        return 1;
    }
    else {
        return 0; //send FAILED
    }

//    if (xBytesSent != sizeof(payload)) {
//        return 0; //send FAILED
//    }
//    else {
//        prvLcdOut();
//        return 1; //send SUCCEEDED
//    }

}
*/

void lcd_task()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U ch;
  INT8U lcd_state = LCD_POWER_UP;


  while (1)
  {
      Q_space = uxQueueSpacesAvailable(Q_LCD);

  switch( lcd_state )
  {
    case LCD_POWER_UP:
      LCD_init = 0;
      lcd_state = LCD_INIT ;

      vTaskDelay(  50 / portTICK_PERIOD_MS);
      break;

    case LCD_INIT:
      if( LCD_init_sequense[LCD_init] != 0xFF )
        wr_ctrl_LCD( LCD_init_sequense[LCD_init++] );
      else
      {
        lcd_state = LCD_READY ;
      }
      vTaskDelay(  50 / portTICK_PERIOD_MS);
      break;

    case LCD_READY:
        if(xQueueReceive( Q_LCD, &ch, portMAX_DELAY  ))
        {
            switch( ch )
            {
              case 0xFF:
                clr_LCD();
                break;
              case ESC:
                lcd_state = LCD_ESC_RECEIVED ;
                break;
              default:
                out_LCD( ch );
                vTaskDelay(  5 / portTICK_PERIOD_MS);
            }
        }
        break;
    case LCD_ESC_RECEIVED:
      if( xQueueReceive( Q_LCD, &ch, portMAX_DELAY ))
      {
        if( ch & 0x80 )
        {
            Set_cursor( ch );
        }
        else
        {
          switch( ch )
          {
            case '@':
                home_LCD();
              break;
          }
        }
        lcd_state = LCD_READY ;
        vTaskDelay(  5 / portTICK_PERIOD_MS);
      }
      break;
      }
  }
}


/****************************** End Of Module *******************************/




