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
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "emp_type.h"
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "lcd.h"
#include "print.h"
#include "digi.h"
#include "main.h"

#include "fueling.h"
#include "payment.h"
#include "fuelsel.h"
#include "pc_terminal.h"

static EventGroupHandle_t xTaskEventGroup = NULL; //non static so we can access it from outside


/*****************************   Functions   *******************************/
EventGroupHandle_t getEvGroup(){
  return xTaskEventGroup;
}
int main(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
    {
  init_gpio();

  EventBits_t uxBits;
  xTaskEventGroup = xEventGroupCreate();
  if( xTaskEventGroup == NULL ) for( ;; ); //insufucent heap for the event group

  BOOLEAN led = red_led_init();       // Status led.
  BOOLEAN print = init_print();       // UART RX/TX Task
  BOOLEAN lcd = init_lcd();           // LCD Display Task
  BOOLEAN keyboard = init_keyboard(); // Keyboard Input Task
  BOOLEAN digi = init_digi();         // Digiswitch Task
  BOOLEAN pc_terminal = init_pc_terminal(); // PC Terminal Task

  BOOLEAN payment = init_payment();   // Payment Task
  BOOLEAN fuelsel = init_fuelsel();   // Fuelselection Task
  BOOLEAN fueling = init_fueling();   // Fueling Task


  if( print && lcd && keyboard && digi && fueling && payment && fuelsel) {
    uxBits = xEventGroupSetBits( xTaskEventGroup, EV_GROUP_payment ); // set bits for next task to be unblocked
    if(uxBits != EV_GROUP_payment){
      uartPrint("ERROR: set of EV_GROUP_fuelsel was not successful\r\n");
    } else {
      uartPrint("EV_GROUP_payment: fuel sel. task allowed to run\r\n");
    }
    vTaskStartScheduler();     /* Start the tasks and timer running. */
  } else {
    for( ;; ); //insufucent heap for one of the tasks, check above flags in debugger
  }




  /* If all is well, the scheduler will now be running, and the following
  line will never be reached.  If the following line does execute, then
  there was insufficient FreeRTOS heap memory available for the idle and/or
  timer tasks to be created.  See the memory management section on the
  FreeRTOS web site for more details. */
  for( ;; );
  return(0);
}









