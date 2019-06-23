#include "Random.h"

/* Override these */
void Random::start() {
  hue = 0;
  for (int i = 0; i < COLS * METERS; i++) {
    twinkles[i] = 0;
  }
}

void Random::loop() {
  hue++;

  // Even values count down, odd counts up.
  for (int meter = 0; meter < COLS * METERS; meter++) {
    uint8_t val = twinkles[meter];
    if (val > 0) {
      if (val == 255) {
        val--;
      } else if (val % 2 == 0) {
        val -= 2;
      } else {
        val += 2;
      }
      twinkles[meter] = val;
    }
  }

  // Generate twinkles.
  if (random(100) <= 10) {
    int newTwinkleIndex = random(COLS * METERS);
    if (twinkles[newTwinkleIndex] == 0) {
      twinkles[newTwinkleIndex] = 1;
    }
  }

  // Set all leds colors.
  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      uint8_t h = hue + getX(col, meter) * 4 + getY(col, meter) * 4;
      uint8_t v = twinkles[METERS * col + meter];
      uint8_t s = 255 - (v / 2);

      columns[col].meterHSV(meter, h, s, v);
    }
  }

  // delay(5);
  show();
}