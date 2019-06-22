#include "Sine.h"

/* Override these */
void Sine::start() {
  hue = 0;
  val = 0;
}

void Sine::loop() {
  // Advance vars for this next frame.
  hue += 1;
  val += 2;

  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      uint8_t x = getX(col, meter);
      uint8_t y = getY(col, meter);
      uint8_t sin = sin8(x - val);

      int distance = constrain(255 - abs(y - sin), 0, 255);

      int val = map(distance, 140, 200, 0, 255);
      val = constrain(val, 0, 255);

      // Give a thin whitened core.
      int sat = map(distance, 200, 255, 255, 64);
      sat = constrain(sat, 0, 255);

      columns[col].meterHSV(meter, hue + x, sat, val);
    }
  }

  show();
  delay(25);
}