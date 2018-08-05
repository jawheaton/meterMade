#include "Rainbow.h"

/* Override these */
void Rainbow::start() {
  hue = 0;
}

void Rainbow::loop() {
  
  // Advance hue for this next frame.
  hue--;
  
  for (int col = 0; col < COLS; col++) {
    // Make the colors whiter if the sensor is triggered.
    // uint8_t sat = sensors[col] ? 64 : 255;
  
    for (int i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
      columns[col].meterHSV(i,
        hue + getX(col, i) + getY(col, i),
        255,
        255
      );
    }
  }
  
  show();
  delay(50);
}