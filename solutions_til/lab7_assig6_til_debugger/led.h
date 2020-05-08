/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: ui.h
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
* 050128  KA    Module created.
*
*****************************************************************************/

#ifndef _LED_H
  #define _LED_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void yellow_led_task(INT8U, INT8U, INT8U, INT8U);
void red_led_task(INT8U, INT8U, INT8U, INT8U);
void green_led_task(INT8U, INT8U, INT8U, INT8U);

/****************************** End Of Module *******************************/
#endif

