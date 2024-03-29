/* Standard includes. */
//#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "print.h"
#include "fuelsel.h"
#include "main.h"
#include <stdlib.h>

static SemaphoreHandle_t xSemaphoreFuelsel = NULL;
static float pFuelselDieselPrice = 8.12;
static float pFuelselLeadfr92Price = 8.49;
static float pFuelselLeadfr95Price = 8.79;
static int fuelSelState = 0;
/*-----------------------------------------------------------*/
// static function declarations. static fns must be declared before first use.
static void prvFuelselTask( void *pvParameters );

/* getter and setters */
float getPrice(int fuel_type){
  float fuelselPrice = -1;
  if( xSemaphoreTake( xSemaphoreFuelsel, portMAX_DELAY ) ){
    switch(fuel_type){
      case DIESEL:
        fuelselPrice = pFuelselDieselPrice;
        break;
      case LEAD_FREE_92:
        fuelselPrice = pFuelselLeadfr92Price;
        break;
      case LEAD_FREE_95:
        fuelselPrice = pFuelselLeadfr95Price;
        break;
    }
    xSemaphoreGive( xSemaphoreFuelsel );
  }
  return fuelselPrice; //-1 if error, value else
}
//char * getFuelTypeName(int fuel_type){
//  switch(fuel_type){
//    case DIESEL:
//      return "Diesel";
//    case LEAD_FREE_92:
//      return "Leadfr. 92";
//    case LEAD_FREE_95:
//      return "Leadfr. 95";
//  }
//}

int setPrice(float fuelselPrice, int fuel_type){
  int success = -1;
  if( xSemaphoreTake( xSemaphoreFuelsel, portMAX_DELAY ) ){
    success = 1;
    switch(fuel_type){
      case DIESEL:
        pFuelselDieselPrice = fuelselPrice;
        break;
      case LEAD_FREE_92:
        pFuelselLeadfr92Price = fuelselPrice;
        break;
      case LEAD_FREE_95:
        pFuelselLeadfr95Price = fuelselPrice;
        break;
    }
    xSemaphoreGive( xSemaphoreFuelsel );
  }
  return success; //-1 if error, 1 else
}


/*-----------------------------------------------------------*/
BOOLEAN init_fuelsel( void ){
  xSemaphoreFuelsel = xSemaphoreCreateMutex();
  if( xSemaphoreFuelsel != NULL ){
    if(xTaskCreate( prvFuelselTask, "fuelsel task", 200, NULL, ( tskIDLE_PRIORITY + 3 ), NULL ) == pdPASS){
      uartPrint("fuelsel initialized\r\n");
      return 1;
    } else {
      uartPrint("ERROR: fuelsel TASK NOT initialized\r\n");
    }
  } else {
    uartPrint("ERROR: fuelsel SEM NOT initialized\r\n");
  }
  return 0;
}


#define FUELSEL_INIT         0
#define FUELSEL_PRICE_DIESEL 1
#define FUELSEL_PRICE_L92    2
#define FUELSEL_PRICE_L95    3
#define FUELSEL_SEL_DIESEL   4
#define FUELSEL_SEL_L92      5
#define FUELSEL_SEL_L95      6
#define FUELSEL_ERROR        7


int getFuelTypeAndReset(){
  switch(fuelSelState){
    case FUELSEL_SEL_DIESEL:
      fuelSelState = FUELSEL_INIT;
      return DIESEL;
    case FUELSEL_SEL_L92:
      fuelSelState = FUELSEL_INIT;
      return LEAD_FREE_92;
    case FUELSEL_SEL_L95:
      fuelSelState = FUELSEL_INIT;
      return LEAD_FREE_95;
    default: //error occureced should only request this function if selection dialog was finished
      //uartPrint("\r\nERROR: requested getFuelTypeAndReset before selection dialog was finished\r\n");
      return -1;
      break;
  }
}
static void waitForFuelSelection(TickType_t xBlockTime){
  unsigned char key = waitForNextKey(xBlockTime);
  if(key == 0){ // no key selected before timeout, display next state
    switch(fuelSelState){
      case FUELSEL_INIT:
        fuelSelState = FUELSEL_PRICE_DIESEL;
        break;
      case FUELSEL_PRICE_DIESEL:
        fuelSelState = FUELSEL_PRICE_L92;
        break;
      case FUELSEL_PRICE_L92:
        fuelSelState = FUELSEL_PRICE_L95;
        break;
      case FUELSEL_PRICE_L95:
        fuelSelState = FUELSEL_PRICE_DIESEL;
        break;
    }
  } else { // we got a key, display selection message
    switch(key){
      case '1':
        fuelSelState = FUELSEL_SEL_DIESEL;
        break;
      case '2':
        fuelSelState = FUELSEL_SEL_L92;
        break;
      case '3':
        fuelSelState = FUELSEL_SEL_L95;
        break;
      default:
        fuelSelState = FUELSEL_ERROR;
        break;
    }
  }
}

static void setEVGroupFueling(EventGroupHandle_t localTaskEventGroup){
  //if done - give to next task:
  EventBits_t uxBits;
  uxBits = xEventGroupClearBits( localTaskEventGroup, EV_GROUP_fuelsel ); // clear current bits first
  if(uxBits != EV_GROUP_fuelsel){
    uartPrint("ERROR: clear of EV_GROUP_fuesel was not successful\r\n");
  }
  uxBits = xEventGroupSetBits( localTaskEventGroup, EV_GROUP_fueling ); // set bits for next task to be unblocked
  if(uxBits != EV_GROUP_fueling){
    uartPrint("ERROR: set of EV_GROUP_fueling was not successful\r\n");
  }
}

static void prvFuelselTask( void *pvParameters )
{

  char line2[32];
  EventBits_t uxBits;
  EventGroupHandle_t localTaskEventGroup = getEvGroup();
  const TickType_t xBlockTime = pdMS_TO_TICKS( 2000 );
  for( ;; ){
    uxBits = xEventGroupWaitBits( localTaskEventGroup, EV_GROUP_fuelsel, pdFALSE, pdTRUE, portMAX_DELAY );
    if(uxBits == EV_GROUP_fuelsel){ // task is unblocked by ev group here
      //getPrice
      switch(fuelSelState){
        case FUELSEL_INIT:
          sendToLcd("Select Fueltype","with Numpad");
          waitForFuelSelection( pdMS_TO_TICKS( 2000 ) ); //a little bit more time for reading the instructions
          break;
        case FUELSEL_PRICE_DIESEL:
          sprintf(line2, "Price: %2.2f", pFuelselDieselPrice);
          sendToLcd("1 Diesel",line2); 
          waitForFuelSelection( xBlockTime );
          break;
        case FUELSEL_PRICE_L92:
          sprintf(line2, "Price: %2.2f", pFuelselLeadfr92Price);
          sendToLcd("2 Leadfr. 92",line2);
          waitForFuelSelection( xBlockTime );
          break;
        case FUELSEL_PRICE_L95:
          sprintf(line2, "Price: %2.2f", pFuelselLeadfr95Price);
          sendToLcd("3 Leadfr. 95", line2);
          waitForFuelSelection( xBlockTime );
          break;
        case FUELSEL_SEL_DIESEL:
          sendToLcd("Diesel selected","");
          vTaskDelay( xBlockTime );
          setEVGroupFueling(localTaskEventGroup);
          break;
        case FUELSEL_SEL_L92:
          sendToLcd("Leadfree 92","selected");
          vTaskDelay( xBlockTime );
          setEVGroupFueling(localTaskEventGroup);
          break;
        case FUELSEL_SEL_L95:
          sendToLcd("Leadfree 95","selected");
          vTaskDelay( xBlockTime );
          setEVGroupFueling(localTaskEventGroup);
          break;
        case FUELSEL_ERROR:
        default:
          sendToLcd("ERROR: allowed","Number 1 to 3!");
          vTaskDelay( xBlockTime );
          fuelSelState = FUELSEL_INIT;
          break;

      }
    }
  }
}

/*-----------------------------------------------------------*/
/*
 * EXAMPLE Code for freeRTOS state machine. I guess fuelsel task should be done in a simmilar manner
 *
#define IDLE          0
#define MENU_OPTIONS  1
#define EDIT_OFFSET   2
#define EDIT_SCALE    3

static void prvUiTask( void *pvParameters )
{
  unsigned char ucReceivedValue;
  char ch2str[2];
  INT8U state = 0;

  INT16U local_offset = 0;
  char offset_buf[9];
  INT16U local_scale = 0;
  char scale_buf[9];
  INT8U digit = 100;

  for( ;; )
  {
    // press any key to enter in menu mode:
    xQueueReceive( xQueueKeyboard, &ucReceivedValue, portMAX_DELAY );

    //TIL: print key for debugging
    uartPrint("\r\nkey: ");
    ch2str[0]=ucReceivedValue; ch2str[1]='\0';
    uartPrint(ch2str);
    uartPrint("\r\n");

    switch(state) {
      case IDLE:
        lcd_send("Options - exit:*",
                 "offset:1 scale:2");
        state = MENU_OPTIONS;
        lcd_take();
        break;
      case MENU_OPTIONS:
        if(ucReceivedValue=='1'){
          lcd_send("type in offset",
                   "submit with #");
          state = EDIT_OFFSET;
        } else if(ucReceivedValue=='2'){
          lcd_send("type in scale",
                   "submit with #");
          state = EDIT_SCALE;
        } else if(ucReceivedValue=='*'){
          lcd_send("exit","");
          state = IDLE;
          lcd_give();
        } else {
          lcd_send("Options - 1: offset",
                   "2: scale, *: exit");
        }
        break;
      case EDIT_OFFSET:
        digit = ucReceivedValue - 48; //48 is ascii offset
        if(digit >= 0 && digit <= 9){
          local_offset *= 10;
          local_offset += digit;
          local_offset %= 10000; //make sure it only uses 5 digits
          uartPrintDec(offset_buf, local_offset, 4);
          lcd_send("new offset:", offset_buf);
        } else if(ucReceivedValue=='#'){
          set_offset(local_offset);
          uartPrintDec(offset_buf, local_offset, 8);
          lcd_send("saved offset:", offset_buf);
          state = IDLE;
          lcd_give();
        } else if(ucReceivedValue=='*'){
          lcd_send("exit",
                   "aborted change");
          local_offset = 0;
          state = IDLE;
          lcd_give();
        }
        break;
      case EDIT_SCALE:
        digit = ucReceivedValue - 48; //48 is ascii offset
        if(digit >= 0 && digit <= 9){
          local_scale *= 10;
          local_scale += digit;
          local_scale %= 10000; //make sure it only uses 5 digits
          uartPrintDec(scale_buf, local_scale, 4);
          lcd_send("new: scale / 100:", scale_buf);
        } else if(ucReceivedValue=='#'){
          float factor = 1;
          factor = (float) local_scale / 100;
          set_scalefactor(factor);
          uartPrintDec(scale_buf, local_scale, 8);
          lcd_send("saved scale:", scale_buf);
          state = IDLE;
          lcd_give();
        } else if(ucReceivedValue=='*'){
          lcd_send("exit",
                   "aborted change");
          local_offset = 0;
          state = IDLE;
          lcd_give();
        }
        break;

    }
    ucReceivedValue=0;
  }
}
*/
/*-----------------------------------------------------------*/
