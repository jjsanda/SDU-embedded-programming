/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: uart.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150228  MoH   Module created.
*
*****************************************************************************/

#ifndef _LCD_H
  #define _LCD_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
int lcd_send(char *line1, char *line2);
BOOLEAN init_lcd( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

