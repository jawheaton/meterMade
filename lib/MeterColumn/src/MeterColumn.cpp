/*
 MeterMade Project
 (c) 2017 Built by Big
 Written by Jim Wheaton
 
 MeterColumn - representation of a single column of the Meter sculpture
 Each Column has 9 meters (7 singles and 1 double meter at the top).
 Each Meter has 4 LEDs
 */

#include "MeterColumn.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif


/**************************** Contruct MeterColumn  ****************************/
MeterColumn::MeterColumn(void) {
  _pDotStars = NULL;
}

MeterColumn::MeterColumn(Adafruit_DotStar* pDotStars) {
  _pDotStars = pDotStars;
}

/***********************************/
void MeterColumn::setDotStars(Adafruit_DotStar* pDotStars) {
  _pDotStars = pDotStars;
}

void MeterColumn::ledRGB(byte ledIndex, byte r, byte g, byte b) {
  if (ledIndex >= NUM_LEDS_PER_COLUMN) return; // bounds check 
  _pDotStars->setPixelColor(ledIndex, r, g, b);
}

void MeterColumn::ledHSV(byte ledIndex, byte h, byte s, byte v) {
  RgbColor rgb = HsvToRgb(HsvColor(h, s, v));
  ledRGB(ledIndex, rgb.r, rgb.g, rgb.b);
}

void MeterColumn::meterRGB(byte meterIndex, byte r, byte g, byte b, byte mask) {
  int ledIndex = meterIndex * NUM_LEDS_PER_METER;
  
  if (mask & 0b0001) ledRGB(ledIndex + 3, r, g, b);
  if (mask & 0b0010) ledRGB(ledIndex + 2, r, g, b);
  if (mask & 0b0100) ledRGB(ledIndex + 1, r, g, b);
  if (mask & 0b1000) ledRGB(ledIndex, r, g, b);
}

void MeterColumn::meterHSV(byte meter, byte h, byte s, byte v, byte mask) {
  RgbColor rgb = HsvToRgb(HsvColor(h, s, v));
  meterRGB(meter, rgb.r, rgb.g, rgb.b, mask);
}