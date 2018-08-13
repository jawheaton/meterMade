#include "Random.h"

/* Override these */
void Random::start() {
}

void Random::loop() {
  byte hue = 0;
  for (int col = 0; col < COLS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) 
    {
      hue = random(0, 256);
      columns[col].meterHSV(i, hue, 255, 255);
    }
  }
  
  show();
}