/***************************************************************
Touchscreen paint: screen painting application derived from Adafruit
example. Enhanced with more colour selection, clear screen and 
variable pen widths using a pressure-sensing zone on the screen.
Also allow for fine width selection through up/down arrows.

Prerequisites:
  3.5" Raspberry Pi tft lcd display (SPI interface)
  Blue pill with stm32duino core
  ST-Link programmer
  USB serial adapter
  Adafruit_GFX_Library
  Adafruit_ILI9486_STM32 driver
  XPT2046 Touchscreen driver
***************************************************************/

#include "Adafruit_ILI9486_STM32.h"
#include <XPT2046_Touchscreen.h>
#include <math.h>

#define TS_CS_PIN  PA3
#define Serial Serial1    //use Serial1 port

#define BOXSIZE 40
#define HALFBOX BOXSIZE/2
#define MIN_PENRADIUS 1
#define MAX_PENRADIUS MIN_PENRADIUS*20

#define TS_MINX 180    //derived from touchscreen calibration
#define TS_MINY 250
#define TS_MAXX 3900
#define TS_MAXY 3900
#define TS_MINZ 800
#define TS_MAXZ 2000

#define BUTTON_X 40    //CLR button specs
#define BUTTON_Y 100
#define BUTTON_W 50
#define BUTTON_H 40
#define BUTTON_TEXTSIZE 2

Adafruit_GFX_Button buttonCLR;
XPT2046_Touchscreen ts(TS_CS_PIN);
Adafruit_ILI9486_STM32 tft;

int oldcolor, currentcolor,PSZx,PSZy,penradius;
int upt,upb,upl,upr,dnt,dnb,dnl,dnr;
bool SMILE;

void setup()
{
  Serial.begin(115200);
  while ( !Serial && (millis() <= 1000) );
  
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  ts.setRotation(1);    //portrait mode
  tft.setRotation(1);   //portrait mode
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  
//draw the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, BLACK);
  tft.fillRect(BOXSIZE*7, 0, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(0, 0, BOXSIZE*8, BOXSIZE, WHITE);    //draw white outline around all boxes
  
//set default color as RED
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);   //draw white outline around default box
  currentcolor = RED;
  SMILE=true;                  //set smile/sad icon 

  penradius=MIN_PENRADIUS+1;   //default pen width
  PSZx=BOXSIZE*9;              //xcoord of Pressure Sensing Zone(PSZ) on screen
  PSZy=HALFBOX;                //ycoord of Pressure Sensing Zone(PSZ) on screen
  
  tft.drawCircle(PSZx,PSZy,HALFBOX,LIGHTGREY);        //draw circular outline of PSZ
  tft.fillCircle(PSZx,PSZy,penradius,currentcolor);   //fill PSZ with pen colour
 
//setup up and down arrow zone boundaries to detect manual pen width selection. Boundaries 
//are relative to the size and coordinates of the arrows.
  upt=0;                //up arrow top bound
  upb=BOXSIZE-22;       //up arrow bottom bound
  upl=(BOXSIZE*10)-10;  //up arrow left bounds
  upr=(BOXSIZE*10)+10;  //up arrow right bounds

  dnt=BOXSIZE-18;       //down arrow top bound
  dnb=BOXSIZE;          //down arrow bottom bound
  dnl=(BOXSIZE*10)-10;  //down arrow left bound
  dnr=(BOXSIZE*10)+10;  //down arrow right bound

//draw up/down arrows according to the boundaries specified.
  tft.fillTriangle(BOXSIZE*10,0,upl,upb,upr,upb,WHITE);        //up arrow
  tft.fillTriangle(BOXSIZE*10,BOXSIZE,dnl,dnt,dnr,dnt,WHITE);  //down arrow
  
  buttonCLR.initButton(&tft,(BOXSIZE*11)+10,20,BUTTON_W,BUTTON_H,WHITE,BLACK,WHITE,"CLR",BUTTON_TEXTSIZE);
  buttonCLR.drawButton();
}

void loop(void) {
  if (ts.bufferEmpty() || !ts.touched()) {    
    return;
  }

  TS_Point p = ts.getPoint();
//Scale detected touchscreen values to tft width/height using calibrated values.
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

//handle CLR button press
  if (buttonCLR.contains(p.x,p.y)) {     
    buttonCLR.press(true);
  }
  if (buttonCLR.justPressed()) {
    buttonCLR.drawButton(true);    //invert button colours
    tft.fillRect(0, BOXSIZE+1, tft.width(), tft.height(), BLACK);   //clear out drawing zone
    buttonCLR.press(false);
    if (buttonCLR.justReleased())
      buttonCLR.drawButton();      //normal colours
  }

//handle PSZ press
  if ((p.x > PSZx-HALFBOX && p.x < PSZx+HALFBOX) && (p.y > PSZy-HALFBOX && p.y < PSZy+HALFBOX)) {
    penradius = map(p.z, TS_MINZ, TS_MAXZ, MIN_PENRADIUS, MAX_PENRADIUS);    //convert p.z to pen widths
    tft.fillCircle(PSZx,PSZy,HALFBOX,BLACK);          //clear PSZ on screen
    tft.drawCircle(PSZx,PSZy,HALFBOX,WHITE);          //draw zone outline
    tft.fillCircle(PSZx,PSZy,penradius,currentcolor); //refresh pen size on screen
    delay(200);
  }

//handle up arrow press
  if ((p.x > upl && p.x < upr) && (p.y > upt && p.y < upb)) {
    if (penradius >= MAX_PENRADIUS)
      penradius=MAX_PENRADIUS;
    else
      penradius++;
    tft.fillCircle(PSZx,PSZy,HALFBOX,BLACK);
    tft.drawCircle(PSZx,PSZy,HALFBOX,WHITE);
    if (penradius < MAX_PENRADIUS)
      tft.fillCircle(PSZx,PSZy,penradius,currentcolor);    //refresh pen size
    else {
      drawSmiley(&tft,PSZx,PSZy,penradius,currentcolor);   //smiley if at max size
    }
    delay(200);
  }

//handle down arrow press
  if ((p.x > dnl && p.x < dnr) && (p.y > dnt && p.y < dnb)) {
    if (penradius <= MIN_PENRADIUS)
      penradius=MIN_PENRADIUS;
    else
      penradius--;
    tft.fillCircle(PSZx,PSZy,HALFBOX,BLACK);
    tft.drawCircle(PSZx,PSZy,HALFBOX,WHITE);
    if (penradius > MIN_PENRADIUS)
      tft.fillCircle(PSZx,PSZy,penradius,currentcolor);   //refresh pen size
    else
      drawStar(&tft,PSZx,PSZy,penradius,currentcolor);    //star if at min size
    delay(200);
  }

//handle colour box press
  if (p.y < BOXSIZE) {        //if p.y is within box bounds
     oldcolor = currentcolor;
     
     if (p.x < BOXSIZE) {     //find which box is pressed by p.x
       currentcolor = RED; 
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);      //and draw new outline of box
     } else if (p.x < BOXSIZE*2) {
       currentcolor = YELLOW;
       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*3) {
       currentcolor = GREEN;
       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*4) {
       currentcolor = CYAN;
       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*5) {
       currentcolor = BLUE;
       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*6) {
       currentcolor = MAGENTA;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*7) {
       currentcolor = BLACK;
       tft.drawRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (p.x < BOXSIZE*8) {
       currentcolor = WHITE;
       tft.drawRect(BOXSIZE*7, 0, BOXSIZE, BOXSIZE, WHITE);
     }

     if (oldcolor != currentcolor) {      //new colour selected?
        if (oldcolor == RED) 
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);      //redraw box to clear previous outline
        if (oldcolor == YELLOW) 
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
        if (oldcolor == GREEN) 
          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
        if (oldcolor == CYAN) 
          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
        if (oldcolor == BLUE) 
          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
        if (oldcolor == MAGENTA) 
          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
        if (oldcolor == BLACK) 
          tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, BLACK);
        if (oldcolor == WHITE) 
          tft.fillRect(BOXSIZE*7, 0, BOXSIZE, BOXSIZE, WHITE);
        tft.drawRect(0, 0, BOXSIZE*8, BOXSIZE, WHITE);  //refresh outline around all boxes
        
        tft.fillCircle(PSZx,PSZy,HALFBOX,BLACK);      //clear PSZ
        tft.drawCircle(PSZx,PSZy,HALFBOX,WHITE);

        if (penradius == MAX_PENRADIUS)
          drawSmiley(&tft,PSZx, PSZy, penradius, currentcolor);   //refresh PSZ
        else if (penradius == MIN_PENRADIUS)
          drawStar(&tft,PSZx, PSZy, penradius, currentcolor);     //refresh PSZ
        else
          tft.fillCircle(PSZx,PSZy,penradius,currentcolor);       //refresh PSZ
      }
  }
  
//draw a circle representing the point x,y returned by the tft sensor.
//Check circle lies within the drawing area bounds.
  if (penradius == MIN_PENRADIUS && (p.y-HALFBOX) > BOXSIZE) {         //special check for star
      drawStar(&tft,p.x, p.y, penradius, currentcolor);                //coz pen width=minimum
  }
  else if (((p.y-penradius) > BOXSIZE) && ((p.y+penradius) < tft.height())) {
    if (penradius == MAX_PENRADIUS)
      drawSmiley(&tft,p.x, p.y, penradius, currentcolor);
    else
      tft.fillCircle(p.x, p.y, penradius, currentcolor);
  }
}

//draw smiley or sad face.
void drawSmiley(Adafruit_GFX *gfx,int x,int y,int penradius,uint16_t currentcolor) {
  int xoffset=3;
  int yoffset=4;
  gfx->fillCircle(x,y,penradius,currentcolor);    //fill the face
  gfx->fillCircle(x-(penradius/xoffset),y-(penradius/yoffset),3,BLACK);  //left eye
  gfx->fillCircle(x+(penradius/xoffset),y-(penradius/yoffset),3,BLACK);  //right eye

  if (SMILE) {   //smiley face
    for (int roffset=7; roffset < 10; roffset++) {     //draw several times for thicker line
      for (double c = -1.15 ; c < 1.2 ; c += 0.05) {   //arc at bottom of circle
        gfx->drawPixel(sin(c)*(penradius-roffset)+x,cos(c)*(penradius-roffset)+y,BLACK);
      }
    }
  }
  else {  //sad face
    for (int roffset=7; roffset < 10; roffset++) {   //draw several times for thicker line
      for (double c = 2.25; c < 4.0; c += 0.05) {    //arc at top of circle shifted downwards
        gfx->drawPixel(sin(c)*(penradius-roffset)+x,cos(c)*(penradius-roffset)+y*2,BLACK);
      }
    }
  }
}

//draw 5-point star.
void drawStar(Adafruit_GFX *gfx,int x,int y,int penradius,uint16_t currentcolor) {
  gfx->drawTriangle(x,y-18,x-4,y-8,x+4,y-8,currentcolor);     //top 
  gfx->drawTriangle(x-18,y-8,x-4,y-8,x-8,y+1,currentcolor);   //left
  gfx->drawTriangle(x+18,y-8,x+4,y-8,x+8,y+1,currentcolor);   //right
  gfx->drawTriangle(x-13,y+15,x-8,y+2,x,y+8,currentcolor);    //bottom left
  gfx->drawTriangle(x+13,y+15,x+8,y+2,x,y+8,currentcolor);    //bottom right
}

