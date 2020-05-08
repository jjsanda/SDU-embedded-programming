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

#ifndef _UART_H
  #define _UART_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void uart_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
void uart_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);


extern void uart0_init( INT32U, INT8U, INT8U, INT8U );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

