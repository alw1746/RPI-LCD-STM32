# RPI-LCD-STM32
Interfacing a Raspberry Pi 3.5 inch LCD to STM32F103C8T6 (blue pill) running stm32duino using SPI.
  ![STM32F103C8T6](/images/bluepill.png)
  ![RPI 3.5 inch LCD](/images/LCD.png)

The LCD normally plugs into the RPi's GPIO header(pin 1-26) using a short female socket. But in this project, the LCD is connected to the STM32F103C8T6 pins using jumper wires. The connections are 5V, Gnd and the SPI pins.

## Wiring diagram
  ![LCD_STM32 wiring](/images/Schematic.png)
  
The FTDI USB-serial adapter is only used for debug output.  
  ![USB-serial adapter](/images/USBSerialAdapter.png)

Redirect Serial to Serial1 in the application sketches.

  **#define Serial Serial1**

The STM32 is programmed using a cheap ST-Link device found in ebay.

![ST-LINK V2](/images/stlinkv2.png)

### Configure the Arduino IDE with STLink as the upload method.

Board: Generic STM32F103C series  
Variant: STM32F103C8 (20k RAM. 128k Flash)  
CPU Speed(MHZ): 72Mhz (Normal)  
Upload method: STLink

## Prerequisite software to drive the display

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
  Download from https://github.com/PaulStoffregen/XPT2046_Touchscreen and configure touchscreen slave select pin in TSpaint.ino:

  **#define TS_CS_PIN  PA3**

## Arduino application sketches
The LCD sketches should be run in the following order to test and obtain information.

1. **graphictest.ino** - generate test patterns on the LCD(see video). This verifies LCD-STM32 wiring is correct. If you get a white screen there is a mixup in the wiring, loose connections, insufficient power, etc.  
<iframe width="854" height="480" src="https://www.youtube.com/embed/hBzeoJun87o" frameborder="0" allowfullscreen></iframe>

2. **LCDcalibrate.ino** - display the boundary and pressure limits(min/max) of the LCD. Plug these values into TSpaint.  
![LCDcalibrate output](/images/LCDcalibrate.jpg)

3. **TSpaint.ino** - enhanced version of Adafruit's touchscreen painter.  
![TSpaint output](/images/TSpaint.jpg)


