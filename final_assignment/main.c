/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: emp.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <scale.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "emp_type.h"
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "lcd.h"
#include "print.h"

//#include "main_blinky.c"
/*****************************    Defines    *******************************/
//#define configCREATE_SIMPLE_TICKLESS_DEMO   1
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
//static QueueHandle_t xQueueUartRX = NULL;
//static QueueHandle_t xQueueUartTX = NULL;

/*****************************   Functions   *******************************/

int main(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  init_gpio();

  BOOLEAN print = init_print();       // UART RX/TX Task
  BOOLEAN lcd = init_lcd();           // LCD Display Task
  BOOLEAN keyboard = init_keyboard(); // Keyboard Input Task

  // BOOLEAN digi = init_digi();         // Digiswitch Task
  // BOOLEAN fueling = init_fueling();   // Fueling Task
  // BOOLEAN payment = init_payment();   // Payment Task
  // BOOLEAN fuelsel = init_fuelsel();   // Fuelselection Task
  // BOOLEAN pc_terminal = init_pc_terminal(); // PC Terminal Task

  //if( ui_keyboard && print && lcd && ai) {
  if( keyboard && print && lcd ) {
    vTaskStartScheduler();     /* Start the tasks and timer running. */
  }

  /* If all is well, the scheduler will now be running, and the following
  line will never be reached.  If the following line does execute, then
  there was insufficient FreeRTOS heap memory available for the idle and/or
  timer tasks to be created.  See the memory management section on the
  FreeRTOS web site for more details. */
  for( ;; );
  return(0);
}









