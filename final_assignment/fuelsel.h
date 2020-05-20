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

#ifndef _FUELSEL_H
  #define _FUELSEL_H

#define DIESEL       0
#define LEAD_FREE_92 1
#define LEAD_FREE_95 2

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
float getPrice(int fuel_type);
int getFuelTypeAndReset();
int setPrice(float fuelselPrice, int fuel_type);
BOOLEAN init_fuelsel( void );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

