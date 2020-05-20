/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"



//DEMO APPPl
/* Standard includes. */
//#include "string.h"
//#include "stdio.h"

/* FreeRTOS includes. */
//#include "task.h"
//#include "semphr.h"

/* Example includes. */
#include "FreeRTOS_CLI.h"

/* Demo application includes. */

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE   50

/* DEL acts as a backspace. */
#define cmdASCII_DEL    ( 0x7F )

/* The maximum time to wait for the mutex that guards the UART to become
available. */
#define cmdMAX_MUTEX_WAIT   pdMS_TO_TICKS( 300 )

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "\r\nFreeRTOS command server.\r\nType Help to view a list of registered commands.\r\n\r\n>";
static const char * const pcEndOfOutputMessage = "\r\n[Press ENTER to execute the previous command again]\r\n>";
static const char * const pcNewLine = "\r\n";

/* Used to guard access to the UART in case messages are sent to the UART from
more than one task. */
static SemaphoreHandle_t xTxMutex = NULL;

/* The handle to the UART port, which is not used by all ports. */
//static xComPortHandle xPort = 0;





























static SemaphoreHandle_t xSemaphorePc_terminal = NULL;
static INT16U pPc_terminalValue = 0;

/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvUARTCommandConsoleTask( void *pvParameters );

/* getter and setters */
INT16U get_pc_terminal(){
  INT16U pc_terminalVal = -1;
  if( xSemaphoreTake( xSemaphorePc_terminal, portMAX_DELAY ) ){
    pc_terminalVal = pPc_terminalValue;
    xSemaphoreGive( xSemaphorePc_terminal );
  }
  return pc_terminalVal; //-1 if error, value else
}


/*-----------------------------------------------------------*/
BOOLEAN init_pc_terminal( void ){
  xTxMutex = xSemaphoreCreateMutex();
  //vRegisterSampleCLICommands();
  vRegisterFuelCLICommands();
  if( xTxMutex != NULL ){
    xTaskCreate( prvUARTCommandConsoleTask, "pc_terminal task", 300, NULL, ( tskIDLE_PRIORITY + 1 ), NULL );
    uartPrint("pc_terminal initialized\r\n");
    return 1;
  } else {
    return 0;
  }
}

static void prvPc_terminalTask( void *pvParameters )
{
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  for( ;; ){
    if( xSemaphoreTake( xSemaphorePc_terminal, 0 ) ){

       //work with pPc_terminalValue

      xSemaphoreGive( xSemaphorePc_terminal );
    }
    vTaskDelay( xBlockTime );
  }
}












/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
static void prvUARTCommandConsoleTask( void *pvParameters )
{
  unsigned char cRxedChar;
  uint8_t ucInputIndex = 0;
  unsigned char *pcOutputString;
  static char cInputString[ cmdMAX_INPUT_SIZE ], cLastInputString[ cmdMAX_INPUT_SIZE ];
  BaseType_t xReturned;
  //xComPortHandle xPort;

  ( void ) pvParameters;

  /* Obtain the address of the output buffer.  Note there is no mutual
  exclusion on this buffer as it is assumed only one command console interface
  will be used at any one time. */
  pcOutputString = FreeRTOS_CLIGetOutputBuffer();

  /* Initialise the UART. */
  //xPort = xSerialPortInitMinimal( configCLI_BAUD_RATE, cmdQUEUE_LENGTH );

  /* Send the welcome message. */
  uartPrint( ( unsigned char * ) pcWelcomeMessage);

  for( ;; )
  {
    /* Wait for the next character.  The while loop is used in case
    INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
    be a genuine block time rather than an infinite block time. */
    while( uartGetChar( &cRxedChar, portMAX_DELAY ) != pdPASS );

    /* Ensure exclusive access to the UART Tx. */
    if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
    {
      /* Echo the character back. */
      uartPutChar( (unsigned char) cRxedChar, portMAX_DELAY );

      /* Was it the end of the line? */
      if( cRxedChar == '\n' || cRxedChar == '\r' )
      {
        /* Just to space the output from the input. */
        //vSerialPutString( xPort, ( signed char * ) pcNewLine, ( unsigned short ) strlen( pcNewLine ) );
        uartPrint((unsigned char *) pcNewLine);

        /* See if the command is empty, indicating that the last command
        is to be executed again. */
        if( ucInputIndex == 0 )
        {
          /* Copy the last command back into the input string. */
          uartStrcpy( cInputString, cLastInputString );
        }

        /* Pass the received command to the command interpreter.  The
        command interpreter is called repeatedly until it returns
        pdFALSE (indicating there is no more output) as it might
        generate more than one string. */
        do
        {
          /* Get the next output string from the command interpreter. */
          xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

          /* Write the generated string to the UART. */
          //vSerialPutString( xPort, ( signed char * ) pcOutputString, ( unsigned short ) strlen( pcOutputString ) );
          uartPrint((unsigned char *) pcOutputString);

          //uartPrint("debug1");

        } while( xReturned != pdFALSE );

        /* All the strings generated by the input command have been
        sent.  Clear the input string ready to receive the next command.
        Remember the command that was just processed first in case it is
        to be processed again. */
        strcpy( cLastInputString, cInputString );
        //uartStrcpy( cLastInputString, cInputString );
        ucInputIndex = 0;
        memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

        //vSerialPutString( xPort, ( signed char * ) pcEndOfOutputMessage, ( unsigned short ) strlen( pcEndOfOutputMessage ) );
        uartPrint((unsigned char *) pcEndOfOutputMessage);
      }
      else
      {
        if( cRxedChar == '\r' )
        {
          /* Ignore the character. */
        }
        else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
        {
          /* Backspace was pressed.  Erase the last character in the
          string - if any. */
          if( ucInputIndex > 0 )
          {
            ucInputIndex--;
            cInputString[ ucInputIndex ] = '\0';
          }
        }
        else
        {
          /* A character was entered.  Add it to the string entered so
          far.  When a \n is entered the complete string will be
          passed to the command interpreter. */
          if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
          {
            if( ucInputIndex < cmdMAX_INPUT_SIZE )
            {
              cInputString[ ucInputIndex ] = cRxedChar;
              ucInputIndex++;
            }
          }
        }
      }

      /* Must ensure to give the mutex back. */
      xSemaphoreGive( xTxMutex );
    }
  }
}

/*-----------------------------------------------------------*/
//void vOutputString( const char * const pcMessage ) //i guess this is used by command functions itself
//{
//  if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
//  {
//    //vSerialPutString( xPort, ( signed char * ) pcMessage, ( unsigned short ) strlen( pcMessage ) );
//    uartPrint((unsigned char *) pcMessage);
//    xSemaphoreGive( xTxMutex );
//  }
//}
/*-----------------------------------------------------------*/
