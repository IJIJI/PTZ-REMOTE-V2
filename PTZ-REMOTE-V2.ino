#include "Arduino.h"



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

  data[0].camNum = 1;
  data[1].camNum = 2;
  data[2].camNum = 3;
  data[3].camNum = 4;

  data[0].tally = program;
  data[2].tally = preview;

}

void loop() {



}


