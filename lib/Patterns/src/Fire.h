#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Fire : public PatBase {
 public:
  uint16_t travel;
  uint8_t hue;
  const static uint8_t noiseData[];

  void start();
  void loop();

  uint8_t noise(uint16_t x, uint16_t y);
};
