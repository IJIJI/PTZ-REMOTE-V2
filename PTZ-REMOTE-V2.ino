

#include "Arduino.h"
#include "commands.h"

#define joyXdeadZone 100
#define joyYdeadZone 100
#define joyZdeadZone 100
// #define joyAdeadZone 20

#define camNum1 1
#define camNum2 2
#define camNum3 3
#define camNum4 4

#define buttonHoldTime 2000

#define sendInterval 150

// !

#define joyXPin PA1 //* Analog Pin
#define joyYPin PA2 //* Analog Pin
#define joyZPin PA3 //* Analog Pin

#define camSelect1Pin PB12
#define camSelect2Pin PB13
#define camSelect3Pin PB14
#define camSelect4Pin PB15

#define midSelect1Pin PA11
#define midSelect2Pin PA12
#define midSelect3Pin PA15
#define midSelect4Pin PB3

//!

#include <Key.h>
#include <Keypad.h>

const uint8_t ROWS = 4; //four rows
const uint8_t COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

uint8_t rowPins[ROWS] = {16, 17, 18, 19}; //connect to the row pinouts of the keypad
uint8_t colPins[COLS] = {20, 21, 22}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// !



enum tallyState {
  none,
  preview,
  program
};

struct camDataStruct {
  uint8_t camNum = 0;
  String camName = "Cam ";
  tallyState tally = none;
};


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

struct joystickStateStruct {
  uint8_t pos = 127;
  uint8_t lastPos = 127;
};
struct dataPackage {

  uint8_t currentCam = 0;

  camDataStruct camData[4];

  // uint8_t joyX = 127;
  // uint8_t joyY = 127;
  // uint8_t joyZ = 127;
  // uint8_t lastJoyX = 127;
  // uint8_t lastJoyY = 127;
  // uint8_t lastJoyZ = 127;

  joystickStateStruct joy[3];

  // modes mode = normal;
  // uint8_t speed = 1;


  // buttonState keypadButton[12] = {Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle}; // Button formatting = none:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9, 0:10, *:11, #:12
  // buttonState keypadButtonLast[12] = {Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle, Idle}; // Button formatting = none:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9, 0:10, *:11, #:12

  buttonStateStruct keypadButton[12]; // Formatting 0:1 1:2 2:3 3:4 4:5 5:6 6:7 7:8 8:9 9:0 10:* 11:#
  buttonStateStruct camSelectButton[4];
  buttonStateStruct midSelectButton[4]; // order is 0:Top Left 1:Bottom Left 2:Top Right 3:Bottom right.
};

dataPackage data;

// !

unsigned long lastSend;

void setup() {

  data.camData[0].camNum = camNum1;
  data.camData[1].camNum = camNum2;
  data.camData[2].camNum = camNum3;
  data.camData[3].camNum = camNum4;

  // camData[0].tally = program;
  // camData[2].tally = preview;

  keypad.setHoldTime(buttonHoldTime);

  Serial.begin(115200);

  lastSend = millis();

}

void loop() {

  data = readInputButtons(data);  // TODO convert to pointer
  data = readInputJoystick(data); // TODO convert to pointer

// !

  if ((data.midSelectButton[2].state == Pressed || data.midSelectButton[2].state == Hold) && (data.midSelectButton[3].state != Pressed || data.midSelectButton[3].state != Hold)){

    data.joy[2].pos = 254;
  }
  else if ((data.midSelectButton[3].state == Pressed || data.midSelectButton[3].state == Hold) && (data.midSelectButton[2].state != Pressed || data.midSelectButton[2].state != Hold)){
    
    data.joy[2].pos = 1;
  }

// !

  for (int x = 0; x < 4; x++){

    if (data.camSelectButton[x].state == Pressed && data.camSelectButton[x].stateLast != Pressed){

      data.currentCam = x;

      break;
    }

  }

// !

  if (data.joy[0].pos != 127 || data.joy[0].pos != data.joy[0].lastPos ||
      data.joy[1].pos != 127 || data.joy[1].pos != data.joy[1].lastPos ||
      data.joy[2].pos != 127 || data.joy[2].pos != data.joy[2].lastPos ){
    
    if (lastSend + sendInterval < millis()){

      uint8_t sendData[5] = {joyUpdateCommand, data.joy[0].pos, data.joy[1].pos, data.joy[2].pos, 127};
      sendCommandRS(data.camData[data.currentCam].camNum, sendData );
    }
  }



  if (data.keypadButton[11].state == Pressed || data.keypadButton[11].state == Hold){ 

    for (int x = 0; x < 10; x++){
      if (data.keypadButton[x].state == Pressed && data.keypadButton[x].stateLast != Pressed){

        uint8_t sendData[] = {writePosCommand, x + 1};
        sendCommandRS(data.camData[data.currentCam].camNum, sendData );

        break;
      }
    }
  }
  else{

    for (int x = 0; x < 10; x++){
      if (data.keypadButton[x].state == Pressed && data.keypadButton[x].stateLast != Pressed){

        uint8_t sendData[] = {callPosCommand, x + 1};
        sendCommandRS(data.camData[data.currentCam].camNum, sendData );

        break;
      }
    }
  }

}

//? Hardware Management vv

dataPackage readInputButtons(dataPackage inData){ // add Pointer to keyboard function.

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

// !

  if (keypad.getKeys()){

    for (int x = 0; x < LIST_MAX; x++){

      if (keypad.key[x].stateChanged){

        uint8_t button;
        switch (keypad.key[x].kchar){

          case '1': 
          button = 0;
          break;

          case '2': 
          button = 1;
          break;

          case '3': 
          button = 2;
          break;

          case '4': 
          button = 3;
          break;

          case '5': 
          button = 4;
          break;

          case '6': 
          button = 5;
          break;

          case '7': 
          button = 6;
          break;

          case '8': 
          button = 7;
          break;

          case '9': 
          button = 8;
          break;

          case '0': 
          button = 9;
          break;

          case '*': 
          button = 10;
          break;

          case '#': 
          button = 11;
          break;
        }

        switch (keypad.key[x].kstate) {
          case PRESSED:
          inData.keypadButton[button].state = Pressed;
          break;

          case HOLD:
          inData.keypadButton[button].state = Hold;
          break;

          case RELEASED:
          inData.keypadButton[button].state = Released;
          break;

          case IDLE:
          inData.keypadButton[button].state = Idle;
        }
      }
    }
  }

// !

  readButton(camSelect1Pin, data.camSelectButton[1]);
  readButton(camSelect2Pin, data.camSelectButton[2]);
  readButton(camSelect3Pin, data.camSelectButton[3]);
  readButton(camSelect4Pin, data.camSelectButton[4]);

  readButton(midSelect1Pin, data.midSelectButton[1]);
  readButton(midSelect2Pin, data.midSelectButton[2]);
  readButton(midSelect3Pin, data.midSelectButton[3]);
  readButton(midSelect4Pin, data.midSelectButton[4]);

// !

  return inData;
}

buttonStateStruct readButton(uint16_t pin, buttonStateStruct inButton){

  if(digitalRead(pin)){
    
    if (inButton.state == Pressed && inButton.lastPress + buttonHoldTime < millis()){
      inButton.state = Hold;

    }
    else if (inButton.state != Pressed){
      inButton.state = Pressed;
      inButton.lastPress = millis();
    }
  }
  else{
    inButton.state = Released;
  }
  
// !

  return inButton;
}


dataPackage readInputJoystick(dataPackage inData){
  
  for(int x = 0; x < 3; x++){
    inData.joy[x].lastPos = inData.joy[x].pos;
  }

// !

  #ifdef joyXPin
    if (analogRead(joyXPin) < 511 - joyXdeadZone){
      data.joy[0].pos = map(analogRead(joyXPin), 0, 511 - joyXdeadZone, 1 , 127);
      // data.joyX = map(analogRead(joyXPin), 0, 1023, 255 , 0);
    }
    else if (analogRead(joyXPin) > 512 + joyXdeadZone){
      data.joy[0].pos = map(analogRead(joyXPin), 512 + joyXdeadZone, 1023, 127 , 254);
    }
    else{
      data.joy[0].pos = 127;
    }

  #endif

  #ifdef joyYPin
    if (analogRead(joyYPin) < 511 - joyYdeadZone){
      data.joy[1].pos = map(analogRead(joyYPin), 0, 511 - joyYdeadZone, 1 , 127);
      // data.joyY = map(analogRead(joyYPin), 0, 1023, 255 , 0);
    }
    else if (analogRead(joyYPin) > 512 + joyYdeadZone){
      data.joy[1].pos = map(analogRead(joyYPin), 512 + joyYdeadZone, 1023, 127 , 254);
    }
    else{
      data.joy[1].pos = 127;
    }

  #endif

  #ifdef joyZPin
    if (analogRead(joyZPin) < 465 - joyZdeadZone){
      data.joy[2].pos = map(analogRead(joyZPin), 0, 511 - joyZdeadZone, 1 , 127);
      // data.joyZ = map(analogRead(joyZPin), 0, 1023, 255 , 0);
    }
    else if (analogRead(joyZPin) > 465 + joyZdeadZone){
      data.joy[2].pos = map(analogRead(joyZPin), 512 + joyZdeadZone, 1024, 127 , 254);
      // data.joyZ = analogRead(joyZPin);
    }
    else{
      data.joy[2].pos = 127;
    }

  #endif

// !

  return inData;
}


//? RS485 Protocol vv


uint8_t remove255(uint8_t inVal){
  if (inVal >= 255)
    inVal = 254;

  return inVal;
}

void sendCommandRS(uint8_t camNum, uint8_t command[]){

  uint8_t commandLength = sizeof(command) / sizeof(command[0]);
  if (commandLength > 11)
    commandLength = 11;
  
  uint8_t message[15];
  
  message[0] = remove255(camNum);
  uint16_t totalMessage = message[0];
  
  for (int x = 0; x <= commandLength; x++){
    message[x+1] = remove255(command[x]);
    totalMessage += command[x];
  }

  message[12] = remove255(totalMessage % 256);
  message[13] = 0xFF;



  Serial.write(message, 14);

}

