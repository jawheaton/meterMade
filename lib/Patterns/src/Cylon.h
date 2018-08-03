#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Cylon : public PatBase {
public:
  uint8_t state;
  
  void start();
  void loop();
};

