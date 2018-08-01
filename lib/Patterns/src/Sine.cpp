#include "Sine.h"

/* Override these */
void Sine::start() {
  hue = 0;
  val = 0;
}

void Sine::loop() {
  // Advance hue for this next frame.
  hue++;
  val++;

  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      uint8_t x = getX(col, meter);
      uint8_t y = getY(col, meter);
      uint8_t sin = sin8(x - val) + 30;

      if (y < sin - 30) {
        columns[col].meterHSV(meter, hue + x, 255, 255);
      } else if (y < sin + 30) {
        uint8_t brightness = map(sin, y - 30, y + 30, 0, 255);
        columns[col].meterHSV(meter, hue + x, 255, brightness);
      } else {
        columns[col].meterRGB(meter, 0, 0, 0);
      }
    }
  }

  show();
  delay(25);
}