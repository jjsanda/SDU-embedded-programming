### Architecture

```mermaid
graph TD

  gsEG(user state event group)

  pay((Payment Task))
  fuelsel((Fuelsel. Task))
  fueling((Fueling Task))
  lcd((LCD Display Task))
  keyboard((Keyboard Input Task))
  digi((Digiswitch Task))
  uart((UART Print Task))
  pc((PC Terminal Task))
  
  keyQ[(Key. In Queue)]
  digiQ[(Digi. In Queue)]
  rxQ[(UART RX Queue)]
  txQ[(UART TX Queue)]

  lcdB>LCD Buffer]
  logB>Fuelling Log Buffer]


  keyQ--waitForNextKey-->fuelsel
  gsEG-->fuelsel
  pc--setPrice-->fuelsel
  fuelsel-->lcdB
  fuelsel--startPumping-->gsEG
  

  uart-->rxQ
  rxQ--getNextLine-->pc
  pc--print-->txQ
  logB--getAllLogs-->pc
  txQ-->uart

  keyboard-->keyQ 
  keyQ--waitForNextKey-->pay
  gsEG-->pay
  digi-->digiQ
  digiQ--getDigiRotation-->pay
  pay-->lcdB
  pay--startFuelSel-->gsEG



  lcdB-->lcd


  gsEG-->fueling
  fueling-->lcdB
  fueling--getPrice-->fuelsel
  fueling--getPaymentOption-->pay
  fueling--getCashBalance-->pay
  fueling--teminateSession-->gsEG
  fueling--appendNewLog-->logB
```