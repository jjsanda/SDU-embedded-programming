
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

#ifndef _EXAMPLE_H
  #define _EXAMPLE_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
unsigned char waitForNextKey(TickType_t xTicksToWait);
INT16U get_example();
INT8U row( INT8U y );
INT8U key_catch( INT8U x, INT8U y);
BOOLEAN init_keyboard( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

