#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Rainbow : public PatBase {
public:
  uint8_t hue;
  
  void start();
  void loop();
};

