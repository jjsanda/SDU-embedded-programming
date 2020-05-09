### Architecture

```mermaid
graph TD

  gsEG(user state event group)

  pay((prvPaymentTask))
  fuelsel((prvFuelselTask))
  fueling((prvFuelingTask))
  lcd((prvLcdOut))
  keyboard((prvKeyboardTask))
  digi((Digiswitch Task))
  uartRX((prvRxTask))
  uartTX((prvTxTask))
  pc((prvPc_terminalTask))
  
  keyQ[(xQueueKeyboard)]
  digiQ[(xQueueDigi)]
  rxQ[(xQueuePrintRX)]
  txQ[(xQueuePrintTX)]
  
  fuelPrices[- pFuelselDieselPrice<br/>- pFuelselLeadfr92Price<br/>- pFuelselLeadfr95Price<br/> <br/>sem: xSemaphoreFuelsel]

  lcdB>xMessageBufferLCD<br/>sem: xSemaphoreLCDSend]
  logB>Fuelling Log Buffer]


  keyQ--waitForNextKey-->fuelsel
  gsEG-->fuelsel
  pc--setPrice-->fuelPrices
  fuelsel---fuelPrices
  fuelsel--sendToLcd-->lcdB
  fuelsel--startPumping-->gsEG
  

  uartRX-->rxQ
  rxQ--getNextLine-->pc
  pc--uartPrint-->txQ
  logB--getAllLogs-->pc
  txQ-->uartTX

  keyboard-->keyQ 
  keyQ--waitForNextKey-->pay
  gsEG-->pay
  digi-->digiQ
  digiQ--getDigiRotation-->pay
  pay--sendToLcd-->lcdB
  pay--startFuelSel-->gsEG



  lcdB-->lcd


  gsEG-->fueling
  fueling--sendToLcd-->lcdB

  fueling--getPrice-->fuelPrices
  
  fueling--getPaymentOption-->pay
  fueling--getCashBalance-->pay
  fueling--teminateSession-->gsEG
  fueling--appendNewLog-->logB
```