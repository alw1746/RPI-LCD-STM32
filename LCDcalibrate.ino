/***************************************************
LCDcalibrate: obtain calibration values for LCD. These are:
 min X,Y,Z
 max X,Y,Z
Z=pressure value of touch screen.

Derived from https://github.com/PaulStoffregen/XPT2046_Touchscreen/blob/master/examples/TouchTest/TouchTest.ino
****************************************************/
 
#include "Adafruit_ILI9486_STM32.h"
#include <XPT2046_Touchscreen.h>
#define CS_PIN  PA3
XPT2046_Touchscreen ts(CS_PIN);

#define Serial Serial1

Adafruit_ILI9486_STM32 tft;

int minx,maxx,miny,maxy,minz,maxz;

void setup()
{
  minx=miny=minz=9999;
  maxx=maxy=maxz=0;
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  ts.begin();
  ts.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.print("Poke me...");
  while ( !Serial && (millis() <= 1000) );
}

void loop(void) {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    tft.fillScreen(BLACK);
    if (p.z < minz) minz=p.z;  if (p.z > maxz) maxz=p.z;
    if (p.x < minx) minx=p.x;  if (p.x > maxx) maxx=p.x;
    if (p.y < miny) miny=p.y;  if (p.y > maxy) maxy=p.y;
    tft.setCursor(100, 120);
    tft.print("min z max = "); tft.print(minz); tft.print(" "); tft.print(p.z); tft.print(" "); tft.print(maxz);
    tft.setCursor(100, 140);
    tft.print("min x max = "); tft.print(minx); tft.print(" "); tft.print(p.x); tft.print(" "); tft.print(maxx);
    tft.setCursor(100, 160);
    tft.print("min y max = "); tft.print(miny); tft.print(" "); tft.print(p.y); tft.print(" "); tft.print(maxy);
    delay(30);
  }
}
