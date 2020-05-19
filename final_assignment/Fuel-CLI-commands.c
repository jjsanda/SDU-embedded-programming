/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


 /******************************************************************************
 *
 * http://www.FreeRTOS.org/cli
 *
 ******************************************************************************/


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "fuelsel.h"


/*
 * The function that registers the commands that are defined within this file.
 */
void vRegisterFuelCLICommands( void );

/*
 * Implements the echo-three-parameters command.
 */
static BaseType_t prvSetDieselCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetLeadFree92Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetLeadFree95Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
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
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xParameterDiesel );
	FreeRTOS_CLIRegisterCommand( &xParameterLeadFree92 );
	FreeRTOS_CLIRegisterCommand( &xParameterLeadFree95 );
}
/*-----------------------------------------------------------*/

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
      sprintf( pcWriteBuffer, "Set diesel to: %2.2f\r\n", price );
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
      sprintf( pcWriteBuffer, "Set lead free 92 to: %2.2f\r\n", price );
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
      sprintf( pcWriteBuffer, "Set lead free 95 to: %2.2f\r\n", price );
      setPrice(price, LEAD_FREE_95);
    }
    return pdFALSE;
}
/*-----------------------------------------------------------*/
