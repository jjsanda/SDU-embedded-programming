/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: ui.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
/*****************************    Defines    *******************************/
#define BS_IDLE           0
#define BS_FIRST_PUSH     1
#define BS_FIRST_RELEASE  2

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
INT8U sw1_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );  // SW1 at PF4
}



void sw1_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  // event = NONE:0, TIMEOUT:1 ,SIGNAL:2, RESET:3
  switch( my_state )
    {
      case BS_IDLE:
        if(sw1_pushed()) {
          set_state(BS_FIRST_PUSH);
        }
        break;
      case BS_FIRST_PUSH:
        if(!sw1_pushed()) {
          set_state(BS_IDLE);
          //signal(SEM_SW1);
          preset_sem(SEM_SW1, 1);
        }
        break;
    }
}


/****************************** End Of Module *******************************/












