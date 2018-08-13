#include "ChaseUp.h"

/* Override these */
void ChaseUp::start() {
    meter = 8;
    hue = 0;
}

void ChaseUp::loop() {
  setAllBlack();
  for (int col = 0; col < COLS; col++) {
      columns[col].meterHSV(meter, hue, 255, 255);
  }
  if (meter == 0)
  {
      meter = 8;
      hue++;
  }
 
  show();
}