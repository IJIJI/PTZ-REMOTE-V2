/*
  displayWrite.h - Display library for the PTZ-Remote - description
  Copyright (c) 2020 IJssel Koster.  All right reserved.
*/

#ifndef displayWrite_h
#define displayWrite_h

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

class displayWrite
{
  public:
    displayWrite(int TFT_CS, int TFT_DC, int TFT_MOSI, int TFT_SCLK, int TFT_RST);

    void update(int cam);

    camData cam[4];

    byte currentCam;
  private:
    // int _CS;
    // int _RST;
    // int _DC;

    // int _SCLK;
    // int _MOSI;
};


#endif