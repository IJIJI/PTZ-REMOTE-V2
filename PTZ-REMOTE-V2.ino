#include "Arduino.h"


// #define camSelect1Pin PA15 // Actually mid select 3

#define camSelect1Pin PB15
#define camSelect2Pin PB12
#define camSelect3Pin PB13
#define camSelect4Pin PB14

#define led PC13

// HardwareSerial Serialaa1(PA10, PA9);

long lastSend;

void setup(void) {

  Serial1.begin(115200);

  pinMode(camSelect1Pin, INPUT_PULLUP);
  pinMode(camSelect2Pin, INPUT_PULLUP);
  pinMode(camSelect3Pin, INPUT_PULLUP);
  pinMode(camSelect4Pin, INPUT_PULLUP);

  pinMode(led, OUTPUT);

}

void loop() {
  if(!digitalRead(camSelect1Pin)){
    digitalWrite(led, HIGH);
    Serial1.println("Hello");
  }
  else{
    digitalWrite(led, HIGH);
  }
}

