#pragma once

// This will load the definition for common Particle variable types
#include "MeterColumn.h"

#define COLS 10
#define METERS 9
#define LEDS_PER_METER 4
#define LEDS LEDS_PER_METER * METERS

class PatBase {
public:
  PatBase();
  
  MeterColumn* columns; // pointer to array of column instances
  boolean* sensors;     // pointer to array of boolean sensor values
  
  void setColumns(MeterColumn* columns);
  void setSensors(bool* sensors);
  void show();
  void setAllBlack();
  
  // Fast 8 bit sine and cosine functions.
  uint8_t sin8(uint8_t theta);
  uint8_t cos8(uint8_t theta);
  
  // Get an 8 bit coordinate for a specific meter.
  uint8_t getX(uint8_t col, uint8_t meter);
  uint8_t getY(uint8_t col, uint8_t meter);
  
  // Override these
  virtual void start();
  virtual void loop();
};

