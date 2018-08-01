#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Sine : public PatBase {
public:
  uint8_t hue;
  uint8_t val;
  
  void start();
  void loop();
};

