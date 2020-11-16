/*
  displayWrite.h - Display library for the PTZ-Remote - implementation
  Copyright (c) 2020 IJssel Koster.  All right reserved.
*/

#include "Arduino.h"
#include "displayWrite.h"

#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>

displayWrite::displayWrite(int TFT_CS, int TFT_DC, int TFT_MOSI, int TFT_SCLK, int TFT_RST)
{
  Adafruit_ST7735 tftDisp = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

  tftDisp.initR(INITR_BLACKTAB);
  update(currentCam);
}


void displayWrite::update(int cam){

  currentCam = cam;

  tftDisp.fillScreen(ST7735_BLACK);

  tftDisp.setTextSize(2);

  for (int x = 0; x < 4; x++){

    if (x == currentCam){
      tftDisp.setTextColor(ST7735_BLACK, ST7735_WHITE);
    }
    else {
      tftDisp.setTextColor(ST7735_WHITE, ST7735_BLACK);
    }
    
    tftDisp.setCursor(10,10 + x * 20);
    tftDisp.print(x + 1);
    tftDisp.setTextColor(ST7735_WHITE);
    tftDisp.print(": ");

    if (currentCam == x){
      if (cam[x].tally == none){
        tftDisp.setTextColor(ST7735_BLACK, ST7735_WHITE);
      }
      else if (cam[x].tally == preview){
        tftDisp.setTextColor(ST7735_BLACK, ST7735_GREEN);
      }
      else if (cam[x].tally == program){
        tftDisp.setTextColor(ST7735_BLACK, ST7735_RED);
      }
    }
    else {
      if (cam[x].tally == none){
        tftDisp.setTextColor(ST7735_WHITE);
      }
      else if (cam[x].tally == preview){
        tftDisp.setTextColor(ST7735_GREEN);
      }
      else if (cam[x].tally == program){
        tftDisp.setTextColor(ST7735_RED);
      }
    }
    
    tftDisp.print(cam[x].camName);
    tftDisp.print(cam[x].camNum);
  }


  const int tallyPos = 90;

  if (cam[currentCam].tally == none){
    tftDisp.setCursor(54, 115);
    tftDisp.setTextSize(4);
    tftDisp.setTextColor(ST7735_WHITE);
    tftDisp.print(cam[currentCam].camNum);
  }
  else if (cam[currentCam].tally == preview){
    tftDisp.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tftDisp.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_GREEN);

    tftDisp.setCursor(54, 115);
    tftDisp.setTextSize(4);
    tftDisp.setTextColor(ST7735_BLACK);
    tftDisp.print(cam[currentCam].camNum);
  }
  else if (cam[currentCam].tally == program){
    tftDisp.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tftDisp.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_RED);

    tftDisp.setCursor(54, 115);
    tftDisp.setTextSize(4);
    tftDisp.setTextColor(ST7735_BLACK);
    tftDisp.print(cam[currentCam].camNum);
  }
}