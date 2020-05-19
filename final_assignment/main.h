/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.h
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
* 100408  KHA    Module created.
*
*****************************************************************************/

#ifndef _MAIN_EV_H
  #define _MAIN_EV_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define EV_GROUP_payment 1
#define EV_GROUP_fuelsel 2
#define EV_GROUP_fueling 4
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
EventGroupHandle_t getEvGroup();
/****************************** End Of Module *******************************/
#endif

