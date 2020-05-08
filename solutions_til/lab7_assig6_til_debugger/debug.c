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
#define IDLE    0
#define P_CHAR  1
#define S_CHAR  2
#define RETURN  3

#define MAXLINE 1024

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/


void debug_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  // event = NONE:0, TIMEOUT:1 ,SIGNAL:2, RESET:3

  /*
  INT8U anzahl_2 = 10;
  unsigned char buf[100];
  gsprintf(buf, "\r\nasdf %5u %5i %5d %5e %5x %5f \r\n",anzahl_2,anzahl_2,anzahl_2,anzahl_2,anzahl_2,anzahl_2);
  print(buf);
  */


  static unsigned char c = '\0';
  if(c!='\0'){
    switch(my_state){
      case IDLE:
        if(c=='p') set_state(P_CHAR);
        break;
      case P_CHAR:
        if(c=='s') set_state(S_CHAR);
        else set_state(IDLE);
        break;
      case S_CHAR:
        if(c=='\r') {
          set_state(IDLE);
          //print("\r\ndebug\r\n");
          print("\r\n");
          debug();
          print("\r\n>>");
        }
        else set_state(IDLE);
        break;
    }

    //print new command line or current char for typing feedback
    if(c=='\r') print("\r\n>>");
    else put_queue( Q_UART_TX, c, 100);
    c='\0';
  }
  get_queue( Q_UART_RX, &c, 1000 );
}


/****************************** End Of Module *******************************/












