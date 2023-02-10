#include "color.h"
// Meter colour schemes
#define RED2RED     0
#define GREEN2GREEN 1
#define BLUE2BLUE   2
#define BLUE2RED    3
#define GREEN2RED   4
#define RED2GREEN   5
#define RAINBOW     6

#define xviolet        10
#define xblue          11
#define xlight_blue    12
#define xgreen         13
#define xyellow_green  14
#define xyellow        15
#define xorange        16
#define xred           17
#define xir            18
#define xClear         19

#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

int d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
int xd1, xd2, xd3, xd4, xd5, xd6, xd7, xd8, xd9, xd10;
int yd1, yd2, yd3, yd4, yd5, yd6, yd7, yd8, yd9, yd10;
String zd1, zd2, zd3, zd4, zd5, zd6, zd7, zd8, zd9, zd10;

int reading = 0;
unsigned long period = 20000;
unsigned long last_time = 0;

void setup(void) {

  Serial.begin(115200);
  if (!as7341.begin()) {
    Serial.println("Could not find AS7341");
    while (1) {
      delay(10);
    }
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);

  tft.begin();

  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setCursor(8, 215);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print (" Vio  Lid  Blu  Cya  Gre  Yel  Ora  Red  NIR  clr");
  tft.setCursor(8, 225);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print (" 415  445  480  515  555  590  630  680  910  nm.");

  as7341.enableLED(true);
}

int MA = 0;
void loop() {
  //LED Range is 4mA to 258mA [ rotary_encode ]
  MA = 5;
  as7341.setLEDCurrent(MA); // 4mA
  if(MA<0)MA=0;
  if(MA>258)MA=258;
  //----------------------------------------------
  if ( millis() - last_time > period + last_time) {
    last_time = millis();
    tft.fillScreen(TFT_BLACK);
    //ESP.restart();
    tft.setRotation(1);
    tft.setCursor(8, 215);
    tft.setTextSize (0);
    tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
    tft.print (" Vio  Lid  Blu  Cya  Gre  Yel  Ora  Red  NIR  clr");
    tft.setCursor(8, 225);
    tft.setTextSize (0);
    tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
    tft.print (" 415  445  480  515  555  590  630  680  910  nm.");
  }



  read_X();

  linearMeter(map(d1, 0, 2048, 0, 20), 20,       10, 5, 25, 3, 20, xviolet);
  linearMeter(map(d2, 0, 2048, 0, 20), 20,  10 + 30, 5, 25, 3, 20, xblue);
  linearMeter(map(d3, 0, 2048, 0, 20), 20,  40 + 30, 5, 25, 3, 20, xlight_blue);
  linearMeter(map(d4, 0, 2048, 0, 20), 20,  70 + 30, 5, 25, 3, 20, xgreen);
  linearMeter(map(d5, 0, 2048, 0, 20), 20, 100 + 30, 5, 25, 3, 20, xyellow_green);
  linearMeter(map(d6, 0, 2048, 0, 20), 20, 130 + 30, 5, 25, 3, 20, xyellow);
  linearMeter(map(d7, 0, 2048, 0, 20), 20, 160 + 30, 5, 25, 3, 20, xorange);
  linearMeter(map(d8, 0, 2048, 0, 20), 20, 190 + 30, 5, 25, 3, 20, xred);
  linearMeter(map(d9, 0, 2048, 0, 20), 20, 220 + 30, 5, 25, 3, 20, xir);
  linearMeter(map(d10, 0, 2048, 0, 20), 20, 250 + 30, 5, 25, 3, 20, RAINBOW);



}

void read_X() {
  if (!as7341.readAllChannels()) {
    Serial.println("Error reading all channels!");
    return;
  }
    tft.setRotation(1);
    tft.setCursor(10, 30);
    tft.setTextSize (0);
    tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
    tft.print ("LED : " + (String)MA + " mA. ");
  d1 = as7341.getChannel(AS7341_CHANNEL_415nm_F1);
  d2 = as7341.getChannel(AS7341_CHANNEL_445nm_F2);
  d3 = as7341.getChannel(AS7341_CHANNEL_480nm_F3);
  d4 = as7341.getChannel(AS7341_CHANNEL_515nm_F4);
  d5 = as7341.getChannel(AS7341_CHANNEL_555nm_F5);
  d6 = as7341.getChannel(AS7341_CHANNEL_590nm_F6);
  d7 = as7341.getChannel(AS7341_CHANNEL_630nm_F7);
  d8 = as7341.getChannel(AS7341_CHANNEL_680nm_F8);
  d10 = as7341.getChannel(AS7341_CHANNEL_CLEAR);
  d9 = as7341.getChannel(AS7341_CHANNEL_NIR);
  
  if(d1>1000){xd1 = d1/1000; yd1 = d1%1000; yd1=yd1/100; zd1=(String)xd1+"."+(String)yd1+"k";}else{zd1=(String)d1;} 
  
  tft.setRotation(1);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.setCursor(10, 42); tft.print (zd1 + " ");
  tft.setCursor(10 + 30, 42); tft.print (zd2 + " ");
  tft.setCursor(10 + 60, 42); tft.print (zd3 + " ");
  tft.setCursor(10 + 90, 42); tft.print (zd4 + " ");
  tft.setCursor(10 + 120, 42); tft.print (zd5 + " ");
  tft.setCursor(10 + 150, 42); tft.print (zd6 + " ");
  tft.setCursor(10 + 180, 42); tft.print (zd7 + " ");
  tft.setCursor(10 + 210, 42); tft.print (zd8 + " ");
  tft.setCursor(10 + 240, 42); tft.print (zd9 + " ");
  tft.setCursor(10 + 270, 42); tft.print (zd10 + " ");

  // Print out the stored values for each channel
  //  Serial.print("F1 415nm : ");
  //  Serial.println(d1);
  //  Serial.print("F2 445nm : ");
  //  Serial.println(d2);
  //  Serial.print("F3 480nm : ");
  //  Serial.println(d3);
  //  Serial.print("F4 515nm : ");
  //  Serial.println(d4);
  //  Serial.print("F5 555nm : ");
  //  Serial.println(d5);
  //  Serial.print("F6 590nm : ");
  //  Serial.println(d6);
  //  Serial.print("F7 630nm : ");
  //  Serial.println(d7);
  //  Serial.print("F8 680nm : ");
  //  Serial.println(d8);
  //  Serial.print("Clear    : ");
  //  Serial.println(d9);
  //  Serial.print("Near IR  : ");
  //  Serial.println(d10);


}


// #########################################################################
//  Draw the linear meter
// #########################################################################
// val =  reading to show (range is 0 to n)
// x, y = position of top left corner
// w, h = width and height of a single bar
// g    = pixel gap to next bar (can be 0)
// n    = number of segments
// s    = colour scheme
void linearMeter(int val, int x, int y, int w, int h, int g, int n, byte s)
{
  tft.setRotation(0);
  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
  // Draw n colour blocks
  for (int b = 1; b <= n; b++) {
    if (val > 0 && b <= val) { // Fill in coloured blocks
      switch (s) {
        case 0: colour = TFT_RED; break; // Fixed colour
        case 1: colour = TFT_GREEN; break; // Fixed colour
        case 2: colour = TFT_BLUE; break; // Fixed colour
        case 3: colour = rainbowColor(map(b, 0, n, 127,   0)); break; // Blue to red
        case 4: colour = rainbowColor(map(b, 0, n,  63,   0)); break; // Green to red
        case 5: colour = rainbowColor(map(b, 0, n,   0,  63)); break; // Red to green
        case 6: colour = rainbowColor(map(b, 0, n,   0, 159)); break; // Rainbow (red to violet)
        //==================================================================
        case 10: colour = TFT_MAGENTA; break;
        case 11: colour = TFT_BLUE; break;
        case 12: colour = TFT_CYAN; break;
        case 13: colour = TFT_GREEN; break;
        case 14: colour = TFT_YELLOWGREEN; break;
        case 15: colour = TFT_YELLOW; break;
        case 16: colour = TFT_DARKORANGE ; break;
        case 17: colour = TFT_RED; break;
        case 18: colour = TFT_BROWN; break;
        case 19: colour = TFT_GREENYELLOW; break;
      }
      tft.fillRect(x + b * (w + g), y, w, h, colour);
    }
    else // Fill in blank segments
    {
      tft.fillRect(x + b * (w + g), y, w, h, TFT_DARKGREY);
    }
  }
}

/***************************************************************************************
** Function name:           rainbowColor
** Description:             Return a 16 bit rainbow colour
***************************************************************************************/
// If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
// from 0 = red through to 127 = blue to 159 = violet
// Extending the range to 0-191 adds a further violet to red band

uint16_t rainbowColor(uint8_t spectrum)
{
  spectrum = spectrum % 192;

  uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour spectrum
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  uint8_t sector = spectrum >> 5;
  uint8_t amplit = spectrum & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}
