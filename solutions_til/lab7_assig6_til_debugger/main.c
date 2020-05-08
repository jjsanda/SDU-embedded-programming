#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "uart.h"
#include "string.h"
#include "string.c"
#include "file.h"
#include "file.c"
#include "rtcs.h"
#include "sw1.h"
#include "led.h"
#include "debug.h"



int main(void)
/*****************************************************************************
*   Input    : NONE
*   Output   : Returns 0
*   Function : Init hardware and then loop forever
******************************************************************************/
{
  init_gpio();
  uart0_init( 9600, 8, 1, 'n' );
  init_rtcs();
  //preset_sem(SEM_SW1,0);
  open_queue( Q_UART_TX );
  open_queue( Q_UART_RX );
  print("\r\n>>");

  start_task( TASK_SW1, sw1_task );
  start_task( TASK_RED_LED, red_led_task );
  start_task( TASK_YELLOW_LED, yellow_led_task );
  start_task( TASK_GREEN_LED, green_led_task );

  start_task( TASK_GREEN_LED, debug_task );

  start_task( TASK_UART_TX, uart_tx_task );
  start_task( TASK_UART_RX, uart_rx_task );


  schedule();
  return( 0 );
}
