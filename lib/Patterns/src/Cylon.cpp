#include "Cylon.h"

/* Override these */
void Cylon::start() {
  state = 0;
}

void Cylon::loop() {
  state++;
  if (state > 5) state = 0;

  uint8_t mask;
  switch (state) {
    case 0: mask = 0b0100; break;
    case 1: mask = 0b0010; break;
    case 2: mask = 0b0001; break;
    case 3: mask = 0b0010; break;
    case 4: mask = 0b0100; break;
    case 5: mask = 0b1000; break;
    default:
      break;
  }

  for (int col = 0; col < COLS; col++) {
    for (int meter = 0; meter < METERS; meter++) {
      columns[col].meterRGB(meter, 0, 0, 0);
      columns[col].meterRGB(
        meter,
        sensors[col] ? 0 : 255,
        sensors[col] ? 255 : 0,
        0,
        mask
      );
    }
  }

  show();
  delay(200);
}