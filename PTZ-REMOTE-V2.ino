#include "Arduino.h"

// #include <Adafruit_GFX.h>    
// #include <Adafruit_ST7735.h> 
// #include <SPI.h>

#include "src\Display_Write\displayWrite.h"


#define button1Pin 21
#define button2Pin 22
#define button3Pin 23
#define button4Pin 24


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


displayWrite display(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

byte activeCam = 0;

void setup(void) {

  Serial.begin(115200);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);



  // display.cam[0].camNum = 1;
  // display.cam[1].camNum = 2;
  // display.cam[2].camNum = 3;
  // display.cam[3].camNum = 4;

  // display.cam[0].tally = program;
  // display.cam[2].tally = preview;

}

void loop() {

  if (digitalRead(button1Pin) == LOW && activeCam != 0){
    activeCam = 0;
    display.update(activeCam);

  }
  else if (digitalRead(button2Pin) == LOW && activeCam != 1){
    activeCam = 1;
    display.update(activeCam);

  }
  else if (digitalRead(button3Pin) == LOW && activeCam != 2){
    activeCam = 2;
    display.update(activeCam);

  }
  else if (digitalRead(button4Pin) == LOW && activeCam != 3){
    activeCam = 3;
    display.update(activeCam);

  }

}

