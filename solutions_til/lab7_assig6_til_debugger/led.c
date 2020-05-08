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
#define YELLOW_OFF 0
#define YELLOW_ON  1

/*****************************   Constants   *******************************/
enum color {
    GREEN = 0x08, // Green LED 0b 1000 when counter value is 1
    YELLOW = 0x04, // Blue LED 0b 0100
    RED = 0x02, //Red LED 0b 0010
    OFF = 0x00, // White LED 0b 1110
};

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void setLed(enum color led_color)
/*****************************************************************************
*   Input    : enum color
*   Output   : void
*   Function : set hw led to the specified colro
******************************************************************************/
{
  GPIO_PORTF_DATA_R &= ~(led_color);                  //turn on led
}
void clearLed(enum color led_color)
/*****************************************************************************
*   Input    : enum color
*   Output   : void
*   Function : set hw led to the specified colro
******************************************************************************/
{
  GPIO_PORTF_DATA_R |= led_color;                  //turn off led
}

void yellow_led_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  switch(my_state){
    case YELLOW_ON:
      //setLedColor(OFF);
      clearLed(YELLOW);
      set_state(YELLOW_OFF);
      wait_sem(SEM_SW1,600);
      break;
    case YELLOW_OFF:
      setLed(YELLOW);
      set_state(YELLOW_ON);
      preset_sem(SEM_LED_ON, 1);
      wait(100);
      break;
  }
}

void red_led_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  if(!(GPIO_PORTF_DATA_R & RED)){
    clearLed(RED);
  } else {
    setLed(RED);
    preset_sem(SEM_LED_ON, 1);
  }
  wait(1000); //muss am ende stehen
}

void green_led_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  if(!(GPIO_PORTF_DATA_R & GREEN)){
    clearLed(GREEN);
  } else {
    setLed(GREEN);
  }
  wait_sem(SEM_LED_ON, WAIT_FOREVER);
}

/****************************** End Of Module *******************************/












