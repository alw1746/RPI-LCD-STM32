# RPI-LCD-STM32
Interfacing a Raspberry Pi 3.5 inch LCD to STM32F103C8T6 (blue pill) running stm32duino using SPI.
  ![STM32F103C8T6](/images/bluepill.png)
  ![RPI 3.5 inch LCD](/images/LCD.png)

The LCD normally plugs into the RPi's GPIO header(pin 1-26) using a short female socket. But in this project, the LCD is connected to the STM32F103C8T6 pins using jumper wires. Socket numbering refers to when LCD is viewed from the PCB side.  
2   1  
4   3  
  :  
26  25

## Wiring diagram
  ![LCD_STM32 wiring](/images/Wiring.png)
### Pin Connections
|STM32 Pin|LCD Skt|Name |
|---------|-------|-----|
|5V       |2      |5V   |
|GND      |6      |GND  |
|PA0      |18     |RS   |
|PA7      |19     |MOSI |
|PA6      |21     |MISO |
|PA1      |22     |RST  |
|PA5      |23     |SCK  |
|PA4      |24     |CS0  |
|PA3      |26     |CS1  |  

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

In Adafruit_ILI9486_STM32.cpp, set SPISettings(32000000) in line 15.

  **Adafruit_ILI9486_STM32::Adafruit_ILI9486_STM32(void) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT), spiSet(SPISettings(32000000)), _trans(0) {}**

### XPT2046 touchscreen driver
  Download from https://github.com/PaulStoffregen/XPT2046_Touchscreen and configure touchscreen slave select pin in TSpaint.ino:

  **#define TS_CS_PIN  PA3**

## Arduino application sketches
The sketches should be run in the following order to test and obtain information about the LCD.

1. **graphictest.ino** - generate test patterns on the LCD. This verifies LCD-STM32 wiring is correct. If you get a white screen there is a mixup in the wiring, loose connections, insufficient power, etc.  
[![graphictest output](/images/grtestvid.png)](https://www.youtube.com/watch?v=hBzeoJun87o&t=2s)

2. **LCDcalibrate.ino** - obtain the screen boundary(x,y) and touch pressure(z) extrema of the LCD by poking the top left/bottom right  corners of the LCD with low/high strength. Home(0,0) is the top left corner in landscape mode, X-axis is the top edge and Y-axis is the left edge.  
![LCDcalibrate output](/images/LCDcalibrate.jpg)

3. **TSpaint.ino** - enhanced version of Adafruit's touchscreen painter. Plug in values returned by the XPT2046 controller. The XY values are mapped by the code to pixel coordinates(480x320).

**#define TS_CS_PIN PA3
#define TS_MINX 180
#define TS_MINY 250  
#define TS_MAXX 3900  
#define TS_MAXY 3900  
#define TS_MINZ 800  
#define TS_MAXZ 2000**

![TSpaint output](/images/TSpaint.jpg)


