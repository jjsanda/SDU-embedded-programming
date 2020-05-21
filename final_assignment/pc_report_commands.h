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

#ifndef _REPORT_COMMANDS_H
  #define _REPORT_COMMANDS_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define MAX_LOG_ENTRYS 1000
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void appendLogEntry(long time, int fuelType, int operatingTime, float totalPrice, unsigned char * accountNumberInput);
void vRegisterReportCLICommands( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

