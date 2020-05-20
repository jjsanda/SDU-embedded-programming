/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS_CLI.h"
#include "fuelsel.h"

static BaseType_t prvSetDieselCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetLeadFree92Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetLeadFree95Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static const CLI_Command_Definition_t xParameterDiesel =
{
  "set-diesel",
  "\r\nset-diesel <price>:\r\n set diesel to price\r\n",
  prvSetDieselCommand, /* The function to run. */
  1 /* The user can enter any number of commands. */
};
static const CLI_Command_Definition_t xParameterLeadFree92 =
{
  "set-leadfr-92",
  "\r\nset-leadfr-92 <price>:\r\n set lead free 92 to price\r\n",
  prvSetLeadFree92Command, /* The function to run. */
  1 /* The user can enter any number of commands. */
};
static const CLI_Command_Definition_t xParameterLeadFree95 =
{
  "set-leadfr-95",
  "\r\nset-diesel <price>:\r\n set lead free 95 to price\r\n",
  prvSetLeadFree95Command, /* The function to run. */
  1 /* The user can enter any number of commands. */
};

void vRegisterFuelCLICommands( void )
{
	FreeRTOS_CLIRegisterCommand( &xParameterDiesel );
	FreeRTOS_CLIRegisterCommand( &xParameterLeadFree92 );
	FreeRTOS_CLIRegisterCommand( &xParameterLeadFree95 );
}
static BaseType_t prvSetDieselCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;
    pcParameter = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParameterStringLength );
    char *eptr;
    float price = strtof(pcParameter, &eptr);
    if(price == 0.0 || *eptr != '\0')
      sprintf( pcWriteBuffer, "Setting diesel failed.\r\nThere are parts in the input string which can not get parsed into a float: %s", eptr );
    else if(price >= 0.0){
      sprintf( pcWriteBuffer, "Set diesel to: %4.2f\r\n", price );
      setPrice(price, DIESEL);
    }
    return pdFALSE;
}
static BaseType_t prvSetLeadFree92Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;
    pcParameter = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParameterStringLength );
    char *eptr;
    float price = strtof(pcParameter, &eptr);
    if(price == 0.0 || *eptr != '\0')
      sprintf( pcWriteBuffer, "Setting lead free 92 failed.\r\nThere are parts in the input string which can not get parsed into a float: %s", eptr );
    else if(price >= 0.0){
      sprintf( pcWriteBuffer, "Set lead free 92 to: %4.2f\r\n", price );
      setPrice(price, LEAD_FREE_92);
    }
    return pdFALSE;
}
static BaseType_t prvSetLeadFree95Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;
    pcParameter = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParameterStringLength );
    char *eptr;
    float price = strtof(pcParameter, &eptr);
    if(price == 0.0 || *eptr != '\0')
      sprintf( pcWriteBuffer, "Setting lead free 95 failed.\r\nThere are parts in the input string which can not get parsed into a float: %s", eptr );
    else if(price >= 0.0){
      sprintf( pcWriteBuffer, "Set lead free 95 to: %4.2f\r\n", price );
      setPrice(price, LEAD_FREE_95);
    }
    return pdFALSE;
}
