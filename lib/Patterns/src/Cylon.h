#pragma once

// This will load the definition for common Particle variable types
#include "PatBase.h"

class Cylon : public PatBase {
 public:
  uint8_t pos;
  uint8_t hue;
  bool climbing;

  void start();
  void loop();

  uint8_t valueForPos(uint8_t ledPos);
};
