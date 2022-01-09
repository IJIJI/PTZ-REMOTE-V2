

#include "Arduino.h"
#include "commands.h"

#define joyXdeadZone 130
#define joyYdeadZone 130
// #define joyXdeadZone 60
// #define joyYdeadZone 60
#define joyZdeadZone 400
// #define joyAdeadZone 20

#define camNum1 1
#define camNum2 2
#define camNum3 3
#define camNum4 4

#define buttonHoldTime 2000

#define sendInterval 30 //todo test with different values
#define sendAfterCenter 3
#define sendSingleCommands 3

#define accelCurve 5.5 // value 0 - 9, 0 being tho most curved and 9 the most linear.


// !

#define joyXPin PA1 //* Analog Pin
#define joyYPin PA2 //* Analog Pin
// #define joyZPin PA3 //* Analog Pin

#define camSelect1Pin PB15
#define camSelect2Pin PB12
#define camSelect3Pin PB13
#define camSelect4Pin PB14

#define midSelect1Pin PA11
#define midSelect2Pin PA15
#define midSelect3Pin PB3
#define midSelect4Pin PB4

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

uint8_t rowPins[ROWS] = {PC15, PB9, PB8, PB1}; //connect to the row pinouts of the keypad
uint8_t colPins[COLS] = {PB0, PC14,PB10}; //connect to the column pinouts of the keypad

// R1 -> pin 6 -> PC15
// R2 -> pin 1 -> PB9
// R3 -> pin 2 -> PB8
// R4 -> pin 4 -> PB1
// C1 -> pin 5 -> PB0
// C2 -> pin 7 -> PC14
// C3 -> pin 3 -> PB10

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

  joystickStateStruct joy[3];
  unsigned long lastCenter = millis();

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

  pinMode(camSelect1Pin, INPUT_PULLUP);
  pinMode(camSelect2Pin, INPUT_PULLUP);
  pinMode(camSelect3Pin, INPUT_PULLUP);
  pinMode(camSelect4Pin, INPUT_PULLUP);

  pinMode(midSelect1Pin, INPUT_PULLUP);
  pinMode(midSelect2Pin, INPUT_PULLUP);
  pinMode(midSelect3Pin, INPUT_PULLUP);
  pinMode(midSelect4Pin, INPUT_PULLUP);

  data.camData[0].camNum = camNum1;
  data.camData[1].camNum = camNum2;
  data.camData[2].camNum = camNum3;
  data.camData[3].camNum = camNum4;

  // camData[0].tally = program;
  // camData[2].tally = preview;

  keypad.setHoldTime(buttonHoldTime);

  Serial1.begin(115200);

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
  else{
    data.joy[2].pos = 127;
  }

// !
  if(
    data.joy[0].lastPos == 127 && data.joy[0].pos == 127 && data.joy[0].pos == data.joy[0].lastPos &&
    data.joy[1].lastPos == 127 && data.joy[0].pos == 127 && data.joy[1].pos == data.joy[1].lastPos &&
    data.joy[2].lastPos == 127 && data.joy[0].pos == 127 && data.joy[2].pos == data.joy[2].lastPos){

    for (int x = 0; x < 4; x++){

      if (data.camSelectButton[x].state == Pressed && data.camSelectButton[x].stateLast != Pressed){

        data.currentCam = x;

        break;
      }

    }
  }

// !

  if (data.joy[0].lastPos != 127 || data.joy[0].pos != 127 || data.joy[0].pos != data.joy[0].lastPos ||
      data.joy[1].lastPos != 127 || data.joy[0].pos != 127 || data.joy[1].pos != data.joy[1].lastPos ||
      data.joy[2].lastPos != 127 || data.joy[0].pos != 127 || data.joy[2].pos != data.joy[2].lastPos ||
      data.lastCenter < sendAfterCenter){
    
    if (lastSend + sendInterval < millis()){

      if (data.joy[0].pos == 127 && data.joy[1].pos == 127 && data.joy[2].pos == 127){
        data.lastCenter++;
      }
      else{
        data.lastCenter = 0;
      }

      uint8_t sendData[5] = {joyUpdateCommand, data.joy[0].pos, data.joy[1].pos, data.joy[2].pos, 127};
      sendCommandRS(data.camData[data.currentCam].camNum, sendData );
    }
  }



  if (data.keypadButton[11].state == Pressed || data.keypadButton[11].state == Hold){ 

    if (data.keypadButton[9].state == Pressed && data.keypadButton[9].stateLast != Pressed){

      uint8_t sendData[] = {setZeroCommand};
      for (int x = 0; x < sendSingleCommands; x++){
        sendCommandRS(data.camData[data.currentCam].camNum, sendData );
      }
    }
    else{

      for (int x = 0; x < 9; x++){
        if (data.keypadButton[x].state == Pressed && data.keypadButton[x].stateLast != Pressed){

          uint8_t sendData[] = {writePosCommand, x + 1};
          for (int x = 0; x < sendSingleCommands; x++){
            sendCommandRS(data.camData[data.currentCam].camNum, sendData );
          }

          break;
        }
      }
    }
  }
  else{

    if (data.keypadButton[9].state == Pressed && data.keypadButton[9].stateLast != Pressed){

      uint8_t sendData[] = {callPosCommand, 0};
      for (int x = 0; x < sendSingleCommands; x++){
        sendCommandRS(data.camData[data.currentCam].camNum, sendData );
      }
    }
    else{

      for (int x = 0; x < 10; x++){
        if (data.keypadButton[x].state == Pressed && data.keypadButton[x].stateLast != Pressed){

          uint8_t sendData[] = {callPosCommand, x + 1};
          for (int x = 0; x < sendSingleCommands; x++){
            sendCommandRS(data.camData[data.currentCam].camNum, sendData );
          }

          break;
        }
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

  inData.camSelectButton[0] = readButton(camSelect1Pin, data.camSelectButton[1]);
  inData.camSelectButton[1] = readButton(camSelect2Pin, data.camSelectButton[2]);
  inData.camSelectButton[2] = readButton(camSelect3Pin, data.camSelectButton[3]);
  inData.camSelectButton[3] = readButton(camSelect4Pin, data.camSelectButton[4]);

  inData.midSelectButton[0] = readButton(midSelect1Pin, data.midSelectButton[1]);
  inData.midSelectButton[1] = readButton(midSelect2Pin, data.midSelectButton[2]);
  inData.midSelectButton[2] = readButton(midSelect3Pin, data.midSelectButton[3]);
  inData.midSelectButton[3] = readButton(midSelect4Pin, data.midSelectButton[4]);

// !

  return inData;
}

buttonStateStruct readButton(uint16_t pin, buttonStateStruct inButton){

  if(digitalRead(pin) == LOW){
    
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
      inData.joy[0].pos = map(analogRead(joyXPin), 0, 511 - joyXdeadZone, 1 , 127);
    }
    else if (analogRead(joyXPin) > 512 + joyXdeadZone){

      inData.joy[0].pos = map(analogRead(joyXPin), 512 + joyXdeadZone, 1023, 127 , 254);
    }
    else{
      inData.joy[0].pos = 127;
    }
    inData.joy[0].pos = retailCurve(inData.joy[0].pos);

  #endif

  #ifdef joyYPin
    if (analogRead(joyYPin) < 511 - joyYdeadZone){
      inData.joy[1].pos = map(analogRead(joyYPin), 0, 511 - joyYdeadZone, 1 , 127);
    }
    else if (analogRead(joyYPin) > 512 + joyYdeadZone){
      inData.joy[1].pos = map(analogRead(joyYPin), 512 + joyYdeadZone, 1023, 127 , 254);
    }
    else{
      inData.joy[1].pos = 127;
    }
    inData.joy[1].pos = retailCurve(inData.joy[1].pos);

  #endif

  #ifdef joyZPin
    if (analogRead(joyZPin) < 465 - joyZdeadZone){
      inData.joy[2].pos = map(analogRead(joyZPin), 0, 511 - joyZdeadZone, 1 , 127);
    }
    else if (analogRead(joyZPin) > 465 + joyZdeadZone){
      inData.joy[2].pos = map(analogRead(joyZPin), 512 + joyZdeadZone, 1024, 127 , 254);
    }
    else{
      inData.joy[2].pos = 127;
    }
    inData.joy[2].pos = retailCurve(inData.joy[2].pos);

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
  for (int x = commandLength + 1; x < 12; x++){ //TODO CHECK ZIS cus command is not long enough
    message[x+1] = 0; 
  }

  message[12] = totalMessage % 255; //checksum
  message[13] = 0xFF;



  lastSend = millis();
  Serial1.write(message, 14);

}



uint8_t retailCurve(uint8_t inVal){
  
  double inValFloat = mapfloat(inVal, 0, 255, -1, 1);
  // double inValFloat = (inVal - 127.0) / 127.0;

  double outValFloat = ( inValFloat *(accelCurve/9.0)+( pow(inValFloat, 5.0 ))*(9.0-accelCurve)/9.0 );
  // double outValFloat = inValFloat;

  return mapfloat(outValFloat, -1, 1, 0, 255);
  
}

double mapfloat(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}