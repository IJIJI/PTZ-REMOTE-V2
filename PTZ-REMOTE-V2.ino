#include "Arduino.h"

#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>


#define button1Pin 7
#define button2Pin 6
#define button3Pin 5
#define button4Pin 4


#define TFT_CS    10
#define TFT_RST   8
#define TFT_DC    9

#define TFT_SCLK 13
#define TFT_MOSI 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

byte currentCam = 0;

enum tallyState {
  none,
  preview,
  program
};

struct camData {
  byte camNum = 0;
  String camName = "Cam ";
  tallyState tally = none;
};

camData data[4];

long lastSend;

void setup(void) {

  Serial.begin(115200);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);


  data[0].tally = program;
  data[2].tally = preview;

  updateScreen();
}

void loop() {

  if (digitalRead(button1Pin) == LOW){
    currentCam = 0;
    updateScreen();
  }
  else if (digitalRead(button2Pin) == LOW){
    currentCam = 1;
    updateScreen();
  }
  else if (digitalRead(button3Pin) == LOW){
    currentCam = 2;
    updateScreen();
  }
  else if (digitalRead(button4Pin) == LOW){
    currentCam = 3;
    updateScreen();
  }

}


void updateScreen() {

  tft.fillScreen(ST7735_BLACK);

  tft.setTextSize(2);

  for (int x = 0; x < 4; x++){

    if (x == currentCam){
      tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    }
    else {
      tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    }
    
    tft.setCursor(10,10 + x * 20);
    tft.print(x + 1);
    tft.setTextColor(ST7735_WHITE);
    tft.print(": ");

    if (currentCam == x){
      if (data[x].tally == none){
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
      }
      else if (data[x].tally == preview){
        tft.setTextColor(ST7735_BLACK, ST7735_GREEN);
      }
      else if (data[x].tally == program){
        tft.setTextColor(ST7735_BLACK, ST7735_RED);
      }
    }
    else {
      if (data[x].tally == none){
        tft.setTextColor(ST7735_WHITE);
      }
      else if (data[x].tally == preview){
        tft.setTextColor(ST7735_GREEN);
      }
      else if (data[x].tally == program){
        tft.setTextColor(ST7735_RED);
      }
    }
    
    tft.print(data[x].camName);
    tft.print(data[x].camNum);
  }


  const int tallyPos = 90;

  if (data[currentCam].tally == none){
  }
  else if (data[currentCam].tally == preview){
    tft.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tft.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_GREEN);
  }
  else if (data[currentCam].tally == program){
    tft.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tft.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_RED);
  }
}