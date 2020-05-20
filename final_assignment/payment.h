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

#ifndef _PAYMENT_H
  #define _PAYMENT_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define NO_PAYMENT_TYPE   0
#define CASH_PAYMENT_TYPE 1
#define CARD_PAYMENT_TYPE 2
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
int getCashSum();
int getPaymentType();
BOOLEAN init_payment( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

