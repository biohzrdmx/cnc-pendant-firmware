#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>

void setup();
void loop();

void redraw();
void drawAxis();
void drawRange();
void interruptHandler();

void onPressedX();
void onPressedY();
void onPressedZ();
void onPressedRange();
void onPressedStop();
void onPressedEncoder();
void onLongPressedX();
void onLongPressedY();
void onLongPressedZ();
void onLongPressedRange();
void onLongPressedStop();
void onLongPressedEncoder();

#endif