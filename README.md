# RPI-LCD-STM32
Interfacing a Raspberry Pi 3.5 inch LCD to STM32F103C8T6 (blue pill) running stm32duino using SPI.

The Raspberry Pi 3.5 inch LCD uses SPI to communicate with the RPi. The LCD plugs into the RPi using a shortened GPIO socket.
The connections used are 5V, Gnd and the SPI pins.

## Wiring diagram:
  ![LCD_STM32 wiring](/images/Schematic.png)
  
The FTDI USB-serial adapter is only used for debug output. 
  ![USB-serial adapter](/images/USBSerialAdapter.PNG)

The Serial port must be redirected to Serial1 in the application
sketches:

  **#define Serial Serial1**

Programming the STM32 is done through the ST-LINK V2 adapter.
![ST-LINK V2](/images/stlinkv2.jpg)

## Configure the Arduino IDE as:

Board: Generic STM32F103C series  
Variant: STM32F103C8 (20k RAM. 128k Flash)  
CPU Speed(MHZ): 72Mhz (Normal)  
Upload method: STLink

## Prerequisite software to drive the display are:

### Adafruit GFX library
  Install from the Arduino library manager.
### Adafruit ILI9486 STM32 driver
  Download from https://github.com/palmerr23/STM32F01-ILI9486-RPi-Driver
  Edit Adafruit_ILI9486_STM32.h and adjust the parameters below according to schematic.

**//Control pins |RS |CS |RST|**  
**#define TFT_CNTRL      GPIOA  
#define TFT_RST        PA1  
#define TFT_RS         PA0  
#define TFT_CS         PA4**

In Adafruit_ILI9486_STM32.cpp, set to SPISettings(32000000) in line 15:

  **Adafruit_ILI9486_STM32::Adafruit_ILI9486_STM32(void) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT), spiSet(SPISettings(32000000)), _trans(0) {}**

### XPT2046 touchscreen driver
  Download from https://github.com/PaulStoffregen/XPT2046_Touchscreen  
  Configure PA3 as CS1:  
  **#define TS_CS_PIN  PA3**

The Arduino sketches should be run in the following order:

1. **graphictest** - generate test patterns on the LCD. This verifies LCD-STM32 wiring is correct.
![graphictest output](/images/graphictest.jpg)

2. **LCDcalibrate** - display the boundary and pressure limits(min/max) of the LCD. Plug these values into TSpaint.
3. **TSpaint** - enhanced version of Adafruit's touchscreen painter.
