/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
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
static LogEntry log[MAX_LOG_ENTRYS];
static int logIndex = 0;
static int logFull = 0;

static TimerHandle_t clock24h = NULL;
#define FULL_DAY_IN_SEC 86400 //=60*60*24
static unsigned long clockSeconds = 0; //we save the whole day in seconds
void callBackFunctionClock(TimerHandle_t timer){
  clockSeconds++;
  if( clockSeconds >= FULL_DAY_IN_SEC ){
    clockSeconds = 0;
  }
}

static BaseType_t prvReportCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvGetTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );


static const CLI_Command_Definition_t xParameterReport =
{
  "report",
  "\r\nreport:\r\n get the report based on the logs\r\n",
  prvReportCommand, /* The function to run. */
  0 /* command argument number */
};
static const CLI_Command_Definition_t xParameterSetTime =
{
  "set-time",
  "\r\nset-time <hours> <minutes> <seconds>:\r\n sets the internal time used for logging\r\n",
  prvSetTimeCommand,
  3
};
static const CLI_Command_Definition_t xParameterGetTime =
{
  "get-time",
  "\r\nget-time:\r\n returns the internal time used for logging\r\n",
  prvGetTimeCommand,
  0
};

void vRegisterReportCLICommands( void )
{
  xLogMutex = xSemaphoreCreateMutex();
	FreeRTOS_CLIRegisterCommand( &xParameterReport );
	FreeRTOS_CLIRegisterCommand( &xParameterSetTime );
	FreeRTOS_CLIRegisterCommand( &xParameterGetTime );
	clock24h = xTimerCreate("Clock", pdMS_TO_TICKS(1000), pdTRUE, NULL, callBackFunctionClock);
	xTimerStart(clock24h, portMAX_DELAY);
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
static BaseType_t setSystemTimeSeconds(int hours, int minutes, int seconds){
  if(hours > 23 || hours < 0) return pdFAIL;
  if(minutes > 59 || minutes < 0) return pdFAIL;
  if(seconds > 59 || seconds < 0) return pdFAIL;
  clockSeconds = (hours * 60 * 60) + (minutes * 60) + seconds;
  return pdPASS;
}
long getSystemTimeSeconds(){
  return clockSeconds;
}
static void deleteLog(){
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



static BaseType_t prvSetTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  const char *pcHours;
  const char *pcMinutes;
  const char *pcSeconds;
  pcHours = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);
  pcMinutes = FreeRTOS_CLIGetParameter(pcCommandString, 2, NULL);
  pcSeconds = FreeRTOS_CLIGetParameter(pcCommandString, 3, NULL);
  int hours = atoi(pcHours);
  int minutes = atoi(pcMinutes);
  int seconds = atoi(pcSeconds);
  if(setSystemTimeSeconds(hours,minutes,seconds)==pdPASS){
    sprintf( pcWriteBuffer, "Changed time successfully \r\n");
  } else {
    sprintf( pcWriteBuffer, "Change unsuccessful. Parameters are out of range:\r\nhours: 0-23\r\nminutes: 0-59\r\nseconds: 0-59\r\n");
  }
  return pdFALSE;
}

static BaseType_t prvGetTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  int secTemp = clockSeconds;
  int hours = secTemp / 60 / 60;
  int minSecTemp = secTemp - (hours * 60 * 60);
  int minutes = minSecTemp / 60;
  int seconds = minSecTemp - (minutes * 60);
  sprintf( pcWriteBuffer, "Current system time is: %02i:%02i:%02i\r\n",hours,minutes,seconds);
  return pdFALSE;
}
