#include "Arduino.h"


#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>


#define button1Pin 21
#define button2Pin 22
#define button3Pin 23
#define button4Pin 24

#define testButton1Pin 15
#define testButton2Pin 19
#define testButton3Pin 20

// #define TFT_CS    10
// #define TFT_RST   8
// #define TFT_DC    9

// #define TFT_SCLK 13
// #define TFT_MOSI 11

// #define TFT_CS    PA3
// #define TFT_RST   PA2
// #define TFT_DC    PA1

// #define TFT_SCLK PA5
// #define TFT_MOSI PA7

#define TFT_CS    40
#define TFT_RST   39
#define TFT_DC    38

#define TFT_SCLK 42
#define TFT_MOSI 44


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

  pinMode(testButton1Pin, INPUT_PULLUP);
  pinMode(testButton2Pin, INPUT_PULLUP);
  pinMode(testButton3Pin, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);


  data[0].camNum = 1;
  data[1].camNum = 5;
  data[2].camNum = 7;
  data[3].camNum = 8;

  data[0].tally = program;
  data[2].tally = preview;


  updateScreen();
}

void loop() {

  if (digitalRead(button1Pin) == LOW && currentCam != 0){
    currentCam = 0;
    updateScreen();

  }
  else if (digitalRead(button2Pin) == LOW && currentCam != 1){
    currentCam = 1;
    updateScreen();

  }
  else if (digitalRead(button3Pin) == LOW && currentCam != 2){
    currentCam = 2;
    updateScreen();

  }
  else if (digitalRead(button4Pin) == LOW && currentCam != 3){
    currentCam = 3;
    updateScreen();

  }

  if (digitalRead(testButton1Pin) == LOW && data[currentCam].tally != program){
    data[currentCam].tally = program;
    updateScreen();
  }
  else if (digitalRead(testButton2Pin) == LOW && data[currentCam].tally != preview){
    data[currentCam].tally = preview;
    updateScreen();
  }
  else if (digitalRead(testButton3Pin) == LOW && data[currentCam].tally != none){
    data[currentCam].tally = none;
    updateScreen();
  }

}


void updateScreen() {

  // tft.fillScreen(ST7735_BLACK);

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
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
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
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      }
      else if (data[x].tally == preview){
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      }
      else if (data[x].tally == program){
        tft.setTextColor(ST7735_RED, ST7735_BLACK);
      }
    }
    
    tft.print(data[x].camName);
    tft.print(data[x].camNum);
  }


  const int tallyPos = 90;

  if (data[currentCam].tally == none){
    tft.fillRect(0, tallyPos, 128, 160-(tallyPos), ST7735_BLACK);

    tft.setCursor(54, 115);
    tft.setTextSize(4);
    tft.setTextColor(ST7735_WHITE);
    tft.print(data[currentCam].camNum);
  }
  else if (data[currentCam].tally == preview){
    tft.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tft.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_GREEN);

    tft.setCursor(54, 115);
    tft.setTextSize(4);
    tft.setTextColor(ST7735_BLACK);
    tft.print(data[currentCam].camNum);
  }
  else if (data[currentCam].tally == program){
    tft.fillRect(0, tallyPos, 128, 2, ST7735_WHITE);
    tft.fillRect(0, tallyPos + 2, 128, 160-(tallyPos + 2), ST7735_RED);

    tft.setCursor(54, 115);
    tft.setTextSize(4);
    tft.setTextColor(ST7735_BLACK);
    tft.print(data[currentCam].camNum);
  }
}