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

#ifndef _PRINT_H
  #define _PRINT_H
#include "emp_type.h"


/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void uartPrint(char *string);
void uartWarning(char *string);
void uartError(char *string);
void uartDebug(char *string);
void uartPrintDec(char * buf, int val, INT8U size);
void uartConcat(char* target, char* source1, char* source2);
BOOLEAN init_print( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

