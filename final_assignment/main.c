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
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "emp_type.h"
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "lcd.h"
#include "print.h"
#include "digi.h"

#include "fueling.h"
#include "payment.h"
#include "fuelsel.h"
#include "pc_terminal.h"

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

  BOOLEAN led = red_led_init();       // Status led.
  BOOLEAN print = init_print();       // UART RX/TX Task
  BOOLEAN lcd = init_lcd();           // LCD Display Task
  BOOLEAN keyboard = init_keyboard(); // Keyboard Input Task
 // BOOLEAN digi = init_digi();         // Digiswitch Task

//  BOOLEAN fueling = init_fueling();   // Fueling Task
//  BOOLEAN payment = init_payment();   // Payment Task
//  BOOLEAN fuelsel = init_fuelsel();   // Fuelselection Task
//  BOOLEAN pc_terminal = init_pc_terminal(); // PC Terminal Task
//
  char test[] = "Hej med dig";
  char test1[] = "1 2 3 4 5 6 7 8 ";
  sendToLcd(test, test1);


  if( print && lcd && keyboard  && led) { //&& digi && fueling && payment && fuelsel && pc_terminal
  //if( print && lcd && keyboard && digi && fueling && payment && fuelsel) {
    vTaskStartScheduler();     /* Start the tasks and timer running. */
  } else {
    for( ;; ); //one of the tasks could not initialized (to less heap for task itself)
  }

  /* If all is well, the scheduler will now be running, and the following
  line will never be reached.  If the following line does execute, then
  there was insufficient FreeRTOS heap memory available for the idle and/or
  timer tasks to be created.  See the memory management section on the
  FreeRTOS web site for more details. */
  for( ;; );
  return(0);
}









