/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS_CLI.h"
#include "fuelsel.h"
#include "print.h"
#include "pc_report_commands.h"



/*
done: The shop owner should be able to set the price
done: logfile: time of day, product, operating time, quantity, total price, account number or "CASH"

query a report:
  containing the total sales of gasoline sorted by product,
  the sum of all cash purchases,
  sum of all account purchases
  the total operating time for the pump
*/


typedef struct LogEntry {
  unsigned long time; //seconds till start
  char fuelType; //use define from fuelsel.h
  unsigned int operatingTime; //in seconds
//  float quantity; //in liters
  float totalPrice; //in DKK
  unsigned char accountNumber[9]; //or credit card number
//  unsigned char * accountNumber;
} LogEntry;

static SemaphoreHandle_t xLogMutex = NULL;
static BaseType_t prvReportCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static LogEntry log[MAX_LOG_ENTRYS];
static int logIndex = 0;
static int logFull = 0;


static const CLI_Command_Definition_t xParameterDiesel =
{
  "report",
  "\r\nreport:\r\n get the report based on the logs\r\n",
  prvReportCommand, /* The function to run. */
  0 /* The user can enter any number of commands. */
};

void vRegisterReportCLICommands( void )
{
  xLogMutex = xSemaphoreCreateMutex();
	FreeRTOS_CLIRegisterCommand( &xParameterDiesel );
	appendLogEntry(15, DIESEL, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, DIESEL, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, LEAD_FREE_92, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, DIESEL, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, LEAD_FREE_92, 100, 30.23f, (unsigned char *) "CASH");
	appendLogEntry(15, DIESEL, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, DIESEL, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, LEAD_FREE_95, 100, 30.23f, (unsigned char *) "CASH");
	appendLogEntry(15, LEAD_FREE_95, 100, 30.23f, (unsigned char *) "11123334");
	appendLogEntry(15, LEAD_FREE_95, 100, 30.23f, (unsigned char *) "11123334");
}


// TODO: add another command, were we can set the current system Time in seconds

long getSystemTimeSeconds(){
  //use system timer here, loop over after 60 * 60 * 24 = 86.400
}
void deleteLog(){
  logIndex = 0;
  logFull = 0;
  //use free for log[i].accountNumber here
}
void appendLogEntry(long time, int fuelType, int operatingTime, float totalPrice, unsigned char * accountNumberInput){
  if( xSemaphoreTake( xLogMutex, portMAX_DELAY ) ){
//    strcpy(log[logIndex].accountNumber, accountNumber, ( size_t ) 8);

    log[logIndex].time = time;
    log[logIndex].fuelType = fuelType;
    log[logIndex].operatingTime = operatingTime;
    log[logIndex].totalPrice = totalPrice;

    //copy each char of accountNumber into logIndex
    for(int i=0; *accountNumberInput!='\0' && i<8; i++ ) log[logIndex].accountNumber[i] = *(accountNumberInput++); //TODO value disappears after this functionappendLogEntry is left/returned
    log[logIndex].accountNumber[9] = '\0';

    // with malloc
//    log[logIndex].accountNumber = (unsigned char *) malloc(sizeof(unsigned char) * 9);
//    if(log[logIndex].accountNumber == NULL) for( ;; ); // no heap left
//    strcpy( log[logIndex].accountNumber, accountNumberInput );

    logIndex++;
    if(logIndex >= MAX_LOG_ENTRYS){
      logIndex = 0;
      logFull = 1;
    }
    if(logFull) uartWarning("Log full!! Rolling over and overwrite oldest messages.");
    xSemaphoreGive( xLogMutex );
  }
}
/*
query a report:
  containing the total sales of gasoline sorted by product,
  the sum of all cash purchases,
  sum of all account purchases
  the total operating time for the pump
*/
static BaseType_t prvReportCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  double salesByDiesel = 0;
  double salesByLeadfr92 = 0;
  double salesByLeadfr95 = 0;

  double cashSum = 0;
  double accountSum = 0;

  unsigned long totalOperatingTime = 0;

  //either read all logs only till logindex or read all logs if log is full
  for(int i=0; i<logIndex || ( logFull && i<MAX_LOG_ENTRYS );i++){
    // calculate: containing the total sales of gasoline sorted by product:
    switch(log[i].fuelType){
      case DIESEL:
        salesByDiesel += log[i].totalPrice;
        break;
      case LEAD_FREE_92:
        salesByLeadfr92 += log[i].totalPrice;
        break;
      case LEAD_FREE_95:
        salesByLeadfr95 += log[i].totalPrice;
        break;
    }

    // the sum of all cash purchases and sum of all account purchases
    if(log[i].accountNumber[0] == 'C'){ //C for CASH, account has only digits
      cashSum += log[i].totalPrice;
    } else {
      accountSum += log[i].totalPrice;
    }

    totalOperatingTime += (unsigned long) log[i].operatingTime;
  }
  sprintf( pcWriteBuffer,
           "Report:\r\n\
Total Sales of gasoline by fuel type:\r\n\
Diesel:       %12.2f DKK\r\n\
Lead free 92: %12.2f DKK\r\n\
Lead free 95: %12.2f DKK\r\n\r\n\
Sum of all cash purchases:    %12.2f DKK\r\n\
Sum of all account purchases: %12.2f DKK\r\n\r\n\
Total operating time for the pump: %i sec\r\n",
           salesByDiesel, salesByLeadfr92, salesByLeadfr95, cashSum, accountSum, totalOperatingTime);
  return pdFALSE;
}
