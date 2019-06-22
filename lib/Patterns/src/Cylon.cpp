#include "Cylon.h"

/* Override these */
void Cylon::start() {
  pos = 0;
  hue = 0;
  climbing = true;
}

void Cylon::loop() {
  pos += 16 * (climbing ? 1 : -1);
  if (pos > 247 - 16) climbing = false;
  if (pos == 0) climbing = true;

  hue++;

  uint8_t val1 = valueForPos(0);
  uint8_t val2 = valueForPos((255 * 1) / 3);
  uint8_t val3 = valueForPos((255 * 2) / 3);
  uint8_t val4 = valueForPos(255);

  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      uint8_t meterHue = hue + getX(col, meter);
      columns[col].meterHSV(meter, meterHue, 200, val1, 0b1000);
      columns[col].meterHSV(meter, meterHue, 200, val2, 0b0100);
      columns[col].meterHSV(meter, meterHue, 200, val3, 0b0010);
      columns[col].meterHSV(meter, meterHue, 200, val4, 0b0001);
    }
  }

  show();

  delay(20);
}

uint8_t Cylon::valueForPos(uint8_t ledPos) {
  int val = 255 - abs(ledPos - pos) * 3;
  val = constrain(val, 0, 255);
  return (val * val) >> 8;
}