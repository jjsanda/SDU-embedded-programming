/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.h
*
* PROJECT....: ECP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

#ifndef _LCD_H
  #define _LCD_H

/***************************** Include files *******************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/*****************************    Defines    *******************************/
// Special ASCII characters
// ------------------------



#define LF      0x0A
#define FF      0x0C
#define CR      0x0D

#define ESC     0x1B
#define HOME    0xFE
#define SECOND_LINE 0x1F

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void wr_str_LCD( INT8U* );
int sendToLcd(char* line1, char* line2);
void move_LCD( INT8U, INT8U );
INT8U wr_ch_LCD( INT8U Ch );



void lcd_task();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

