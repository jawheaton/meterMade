#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Random : public PatBase {
 public:
  uint8_t hue;
  uint8_t twinkles[360];
  bool lowPower;

  void start();
  void loop();
};
