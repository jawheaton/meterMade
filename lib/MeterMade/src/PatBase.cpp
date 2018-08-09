#include "PatBase.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif


/**************************** Contruct PatBase  ****************************/
PatBase::PatBase(void) {
  columns = NULL;
  sensors = NULL;
}

/***********************************/
void PatBase::setColumns(MeterColumn* newColumns) {
  columns = newColumns;
}

void PatBase::setSensors(bool* newSensors) {
  sensors = newSensors;
}

// FastLED sin8
// https://github.com/FastLED/FastLED/blob/03d12093a92ee2b64fabb03412aa0c3e4f6384dd/lib8tion/trig8.h#L217
uint8_t PatBase::sin8(uint8_t theta){
  uint8_t b_m16_interleave[] = { 0, 49, 49, 41, 90, 27, 117, 10 };
  uint8_t offset = theta;
  if( theta & 0x40 ) {
      offset = (uint8_t)255 - offset;
  }
  offset &= 0x3F; // 0..63

  uint8_t secoffset  = offset & 0x0F; // 0..15
  if( theta & 0x40) secoffset++;

  uint8_t section = offset >> 4; // 0..3
  uint8_t s2 = section * 2;
  const uint8_t* p = b_m16_interleave;
  p += s2;
  uint8_t b   =  *p;
  p++;
  uint8_t m16 =  *p;

  uint8_t mx = (m16 * secoffset) >> 4;

  int8_t y = mx + b;
  if( theta & 0x80 ) y = -y;

  y += 128;

  return y;
}

// FastLED cos8
uint8_t PatBase::cos8(uint8_t theta) {
  return sin8(theta + 64);
}

uint8_t PatBase::getX(uint8_t col, uint8_t meter) {
  uint8_t offset = 0;
  if (meter <= 1) offset = 255 / COLS / 3;
  if (meter == 0) offset = -offset;

  return offset + (col * 255) / COLS;
}

uint8_t PatBase::getY(uint8_t col, uint8_t meter) {
  if (meter <= 1) return 255;
  return ((METERS - meter - 1) * 255) / (METERS - 2);
}

void PatBase::setAllBlack() {
  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++)
    {
      columns[col].meterRGB(meter, 0,0,0);
    }
  }
}

void PatBase::show() {
  for (int col = 0; col < COLS; col++) {
    columns[col].leds->show();
  }
}

/* Override these */
void PatBase::start() {
  //...
}

void PatBase::loop() {
  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      columns[col].meterRGB(meter, 255, 0, 0);
    }
  }

  show();
  delay(50);
}