#include "Arduino.h"


#define joyXdeadZone 20
#define joyYdeadZone 20
#define joyZdeadZone 20
#define joyAdeadZone 20

#define camNum1 1
#define camNum2 2
#define camNum3 3
#define camNum4 4

// !

#define joyXPin 7 //* Analog Pin
#define joyYPin 6 //* Analog Pin
#define joyZPin 5 //* Analog Pin

#define camSelect1Pin 8
#define camSelect2Pin 9
#define camSelect3Pin 10
#define camSelect4Pin 11

#define midSelect1Pin 12
#define midSelect2Pin 13
#define midSelect3Pin 14
#define midSelect4Pin 15

//!

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, A1, A2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// !

byte currentCam = 0;

enum tallyState {
  none,
  preview,
  program
};

struct camDataStruct {
  byte camNum = 0;
  String camName = "Cam ";
  tallyState tally = none;
};

camDataStruct camData[4];


enum buttonStateEnum {
  Pressed,
  Hold,
  Released,
  Idle
};

struct buttonStateStruct {
  buttonStateEnum state = Idle;
  buttonStateEnum stateLast = Idle;
  unsigned long lastPress = millis();
};

struct dataPackage {
  byte joyX = 127;
  byte joyY = 127;
  byte joyZ = 127;
  byte lastJoyX = 127;
  byte lastJoyY = 127;
  byte lastJoyZ = 127;

  // modes mode = normal;
  // byte speed = 1;


  // buttonState keypadButton[12] = {Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle}; // Button formatting = none:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9, 0:10, *:11, #:12
  // buttonState keypadButtonLast[12] = {Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle}; // Button formatting = none:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9, 0:10, *:11, #:12

  buttonStateStruct keypadButton[12];
  buttonStateStruct camSelectButton[4];
  buttonStateStruct midSelectButton[4]; // order is 0:Top Left 1:Bottom Left 2:Top Right 3:Bottom right.
};

dataPackage data;

// !

unsigned long lastSend;

void setup() {

  camData[0].camNum = camNum1;
  camData[1].camNum = camNum2;
  camData[2].camNum = camNum3;
  camData[3].camNum = camNum4;

  // camData[0].tally = program;
  // camData[2].tally = preview;

  keypad.setHoldTime(2000);

  Serial.begin(115200);

  lastSend = millis();

}

void loop() {



}

//? Hardware Management vv

dataPackage readInputs(dataPackage inData){

  // Update all last states of the keypad
  for(int x = 0; x < 12; x++){
    inData.keypadButton[x].stateLast = inData.keypadButton[x].state;
  }

  // Update all last states of the camera selector buttons
  for(int x = 0; x < 4; x++){
    inData.camSelectButton[x].stateLast = inData.camSelectButton[x].state;
  }

  // Update all last states of the middle buttons
  for(int x = 0; x < 4; x++){
    inData.midSelectButton[x].stateLast = inData.midSelectButton[x].state;
  }


  return inData;
}



//? RS485 Protocol vv


byte remove255(byte inVal){
  if (inVal >= 255)
    inVal = 254;

  return inVal;
}

void sendCommandRS(byte camNum, byte command[]){

  byte commandLength = sizeof(command) / sizeof(command[0]);
  if (commandLength > 11)
    commandLength = 12;
  
  byte message[15];
  
  message[0] = remove255(camNum);
  int totalMessage = message[0];
  
  for (int x = 0; x < commandLength; x++){
    message[x+1] = remove255(command[x]);
    totalMessage += command[x];
  }

  message[13] = remove255(totalMessage % 256);
  message[14] = 0xFF;



  Serial.write(message, 15);

}

