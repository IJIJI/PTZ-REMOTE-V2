
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN     PA11

Adafruit_NeoPixel leds(5, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  leds.begin();
  leds.show();
}

void loop() {
  leds.clear();

  for (int i = 0; i <= 4; i++) {
    leds.setPixelColor(i, leds.Color(255, 0, 0));
    leds.show();
    delay(40);
  }
  for (int i = 0; i <= 4; i++) {
    leds.setPixelColor(i, leds.Color(0, 255, 0));
    leds.show();
    delay(40);
  }
  for (int i = 0; i <= 4; i++) {
    leds.setPixelColor(i, leds.Color(0, 0, 255));
    leds.show();
    delay(40);
  }
}