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

#ifndef _FUELING_H
  #define _FUELING_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define INIT                0
#define IDLE                1
#define START_FUELING       2
#define FUELING             3
#define END_FUELING         4
#define FUELING_TERMINATE   5
#define LIFTED_NOZZLE       6

#define lever       (GPIO_PORTF_DATA_R & 0x01)
#define nozzleBoot  (GPIO_PORTF_DATA_R & 0x10)
//#define greenLED    0x06
//#define redLED      0x0C
//#define yellowLED   0x0A
//#define ledOFF      0x0E
#define greenLED    0xF7
#define redLED      0xFD
#define yellowLED   0xFB
#define ledOFF      0xFF


/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
INT16U get_fueling();
BOOLEAN init_fueling( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

