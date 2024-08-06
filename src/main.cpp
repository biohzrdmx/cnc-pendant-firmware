#include "main.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <YetAnotherPcInt.h>
#include <EasyButton.h>
#include <RotaryEncoder.h>
#include <Keyboard.h>
#include "Timer.h"

#define DIRECTION_CW   1
#define DIRECTION_CCW -1

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH   128 
#define SCREEN_HEIGHT  64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_DEBOUNCE   40

#define PIN_BTN_X      4
#define PIN_BTN_Y      5
#define PIN_BTN_Z      6
#define PIN_BTN_RANGE  10
#define PIN_BTN_STOP   16
EasyButton btnX(PIN_BTN_X, BTN_DEBOUNCE, true);
EasyButton btnY(PIN_BTN_Y, BTN_DEBOUNCE, true);
EasyButton btnZ(PIN_BTN_Z, BTN_DEBOUNCE, true);
EasyButton btnRange(PIN_BTN_RANGE, BTN_DEBOUNCE, true);
EasyButton btnStop(PIN_BTN_STOP, BTN_DEBOUNCE, true);

#define PIN_ENCODER_BTN 7
#define PIN_ENCODER_A   8
#define PIN_ENCODER_B   9
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B);
EasyButton btnEncoder(PIN_ENCODER_BTN, BTN_DEBOUNCE, true);
int oldPos = 0;

#define RANGE_0001 0
#define RANGE_001  1
#define RANGE_01   2
#define RANGE_1    3
#define RANGE_10   4
unsigned int range = RANGE_1;

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
unsigned int axis = AXIS_X;

#define PLATFORM_WIN 0
#define PLATFORM_MAC 1
unsigned int platform = PLATFORM_WIN;

#define STATE_IDLE   0
#define STATE_ACTION 1
unsigned int state = STATE_IDLE;

#define ACTION_NONE      0
#define ACTION_X         1
#define ACTION_Y         2
#define ACTION_Z         3
#define ACTION_ZERO_X    4
#define ACTION_ZERO_Y    5
#define ACTION_ZERO_Z    6
#define ACTION_RANGE     7
#define ACTION_STOP      8
#define ACTION_HOME_XY   9
#define ACTION_RESET     10
#define ACTION_UNLOCK    11
#define ACTION_ZERO_ALL  12
#define ACTION_STEP_UP   13
#define ACTION_STEP_DOWN 14
unsigned int action = ACTION_NONE;

Timer tmrReset;

bool update = false;
bool reset = true;

void setup() {
  Serial.begin(9600);
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  delay(500);
  Serial.println(F("CNC JOG CONTROLLER"));
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawRoundRect(1, 1, 127, 63, 4, SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print(F("CNC JOG CONTROLLER"));
  display.setCursor(48, 34);
  display.print(F("v.1.0"));
  display.display();
  // Initialize buttons
  btnX.begin();
  btnY.begin();
  btnZ.begin();
  btnRange.begin();
  btnStop.begin();
  btnEncoder.begin();
  // Press handlers
  btnX.onPressed(onPressedX);
  btnY.onPressed(onPressedY);
  btnZ.onPressed(onPressedZ);
  btnRange.onPressed(onPressedRange);
  btnStop.onPressed(onPressedStop);
  btnEncoder.onPressed(onPressedEncoder);
  // Long-press handlers
  btnX.onPressedFor(2000, onLongPressedX);
  btnY.onPressedFor(2000, onLongPressedY);
  btnZ.onPressedFor(2000, onLongPressedZ);
  btnRange.onPressedFor(2000, onLongPressedRange);
  btnStop.onPressedFor(2000, onLongPressedStop);
  btnEncoder.onPressedFor(2000, onLongPressedEncoder);
  // Setup timers
  tmrReset.init(3000);
  // Initialize keyboard
  Keyboard.begin();
  // Attach interrupts
  PcInt::attachInterrupt(PIN_ENCODER_A, interruptHandler, CHANGE);
  PcInt::attachInterrupt(PIN_ENCODER_B, interruptHandler, CHANGE);
}

void drawCenteredString(const String &buf, int x, int y = -1)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  display.setCursor((x - w) / 2, y == -1 ? (y - h) / 2 : y);
  display.print(buf);
}

void loop() {
  tmrReset.update();
  btnX.read();
  btnY.read();
  btnZ.read();
  btnRange.read();
  btnStop.read();
  btnEncoder.read();

  int newPos = encoder.getPosition();
  if (newPos != oldPos) {
    RotaryEncoder::Direction dir = encoder.getDirection();
    if (dir == RotaryEncoder::Direction::CLOCKWISE) {
      action = ACTION_STEP_UP;
      update = true;
      tmrReset.restart();
      reset = true;
      switch(axis) {
        case AXIS_X:
          Keyboard.press(KEY_RIGHT_ARROW);
          Keyboard.releaseAll();
        break;
        case AXIS_Y:
          Keyboard.press(KEY_DOWN_ARROW);
          Keyboard.releaseAll();
        break;
        case AXIS_Z:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.press(KEY_DOWN_ARROW);
          Keyboard.releaseAll();
        break;
      }
    } else if (dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
      action = ACTION_STEP_DOWN;
      update = true;
      tmrReset.restart();
      reset = true;
      switch(axis) {
        case AXIS_X:
          Keyboard.press(KEY_LEFT_ARROW);
          Keyboard.releaseAll();
        break;
        case AXIS_Y:
          Keyboard.press(KEY_UP_ARROW);
          Keyboard.releaseAll();
        break;
        case AXIS_Z:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.press(KEY_UP_ARROW);
          Keyboard.releaseAll();
        break;
      }
    }
    oldPos = newPos;
  }
  //
  redraw();
  //
  if ( tmrReset.hasFinished() && reset ) {
    action = ACTION_NONE;
    reset = false;
    display.clearDisplay();
    display.display();
  }
}

void interruptHandler() {
  encoder.tick();
}

void redraw() {
  if (update) {
    display.clearDisplay();
    display.drawRoundRect(1, 1, 127, 63, 4, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    switch(action) {
      case ACTION_X:
        display.print(F("CHANGE AXIS > X"));
      break;
      case ACTION_Y:
        display.print(F("CHANGE AXIS > Y"));
      break;
      case ACTION_Z:
        display.print(F("CHANGE AXIS > Z"));
      break;
      case ACTION_RANGE:
        display.print(F("CHANGE STEP SIZE"));
      break;
      case ACTION_STOP:
        display.print(F("STOP"));
      break;
      case ACTION_ZERO_X:
        display.print(F("RESET X ZERO"));
      break;
      case ACTION_ZERO_Y:
        display.print(F("RESET Y ZERO"));
      break;
      case ACTION_ZERO_Z:
        display.print(F("RESET Z ZERO"));
      break;
      case ACTION_HOME_XY:
        display.print(F("RETURN TO XY ZERO"));
      break;
      case ACTION_RESET:
        display.print(F("SOFT RESET"));
      break;
      case ACTION_STEP_DOWN:
        display.print(F("STEP DOWN"));
      break;
      case ACTION_STEP_UP:
        display.print(F("STEP UP"));
      break;
      case ACTION_ZERO_ALL:
        display.print(F("RESET XYZ ZERO"));
      break;
      case ACTION_UNLOCK:
        display.print(F("UNLOCK"));
      break;
    }
    display.drawLine(1, 42, 127, 42, SSD1306_WHITE);
    display.drawLine(96, 42, 96, 63, SSD1306_WHITE);
    drawAxis();
    drawRange();
    display.display();
    update = false;
  }
}

void drawAxis() {
  display.setCursor(109, 50);
  switch(axis) {
    case AXIS_X:
      display.print(F("X"));
    break;
    case AXIS_Y:
      display.print(F("Y"));
    break;
    case AXIS_Z:
      display.print(F("Z"));
    break;
  }
}

void drawRange() {
  display.setCursor(10, 50);
  switch(range) {
    case RANGE_0001:
      display.print(F("0.001 MM"));
    break;
    case RANGE_001:
      display.print(F("0.01 MM"));
    break;
    case RANGE_01:
      display.print(F("0.1 MM"));
    break;
    case RANGE_1:
      display.print(F("1 MM"));
    break;
    case RANGE_10:
      display.print(F("10 MM"));
    break;
  }
}

void onPressedX() {
  action = ACTION_X;
  update = true;
  tmrReset.restart();
  reset = true;
  axis = AXIS_X;
}

void onPressedY() {
  action = ACTION_Y;
  update = true;
  tmrReset.restart();
  reset = true;
  axis = AXIS_Y;
}

void onPressedZ() {
  action = ACTION_Z;
  update = true;
  tmrReset.restart();
  reset = true;
  axis = AXIS_Z;
}

void onPressedRange() {
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  switch(range) {
    case RANGE_0001:
      range = RANGE_001;
      Keyboard.press('1');
    break;
    case RANGE_001:
      range = RANGE_01;
      Keyboard.press('2');
    break;
    case RANGE_01:
      range = RANGE_1;
      Keyboard.press('3');
    break;
    case RANGE_1:
      range = RANGE_10;
      Keyboard.press('4');
    break;
    case RANGE_10:
      range = RANGE_0001;
      Keyboard.press('5');
    break;
  }
  Keyboard.releaseAll();
  action = ACTION_RANGE;
  update = true;
  tmrReset.restart();
  reset = true;
}

void onPressedStop() {
  action = ACTION_STOP;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('t');
  Keyboard.releaseAll();
}

void onPressedEncoder() {
  action = ACTION_UNLOCK;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('u');
  Keyboard.releaseAll();
}

void onLongPressedX() {
  action = ACTION_ZERO_X;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('x');
  Keyboard.releaseAll();
}

void onLongPressedY() {
  action = ACTION_ZERO_Y;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('y');
  Keyboard.releaseAll();
}

void onLongPressedZ() {
  action = ACTION_ZERO_Z;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('z');
  Keyboard.releaseAll();
}

void onLongPressedRange() {
  action = ACTION_HOME_XY;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('h');
  Keyboard.releaseAll();
}

void onLongPressedStop() {
  action = ACTION_RESET;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('r');
  Keyboard.releaseAll();
}

void onLongPressedEncoder() {
  action = ACTION_ZERO_ALL;
  update = true;
  tmrReset.restart();
  reset = true;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('a');
  Keyboard.releaseAll();
}