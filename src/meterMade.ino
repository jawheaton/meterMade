#include "MeterColumn.h"

#define NUM_COLUMNS 10
#define LED_BRIGHTNESS 32

#define LED_PWR D7
#define LED_DAT C0
#define LED_CLK1 C1
#define LED_CLK2 C2
#define LED_CLK3 C3
#define LED_CLK4 C4
#define LED_CLK5 C5
#define LED_CLK6 D0
#define LED_CLK7 D1
#define LED_CLK8 D2
#define LED_CLK9 D3
#define LED_CLK10 D4

#define RNG10 A3

Adafruit_DotStar strips[] = {
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK1),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK2),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK3),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK4),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK5),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK6),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK7),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK8),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK9),
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK10)
};

MeterColumn columns[NUM_COLUMNS];

uint8_t mode = 02;
bool ledPower = false;

void setup() {
  Serial.begin(9600);
  for (int i=0; i<NUM_COLUMNS; i++)
  {
	  columns[i].SetDotStars(&strips[i]);
  }

  
  // Expose cloud functions.
  Particle.function("TurnOff", turnOff);

  // Every pattern should register a start funtion here.
  Particle.function("Rainbow", rainbow_start);
  Particle.function("Twinkles", twinkles_start);
  Particle.function("RangerDebug", rangerDebug_start);
  Particle.function("Cylon", cylon_start);

  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].begin();
    strips[i].setBrightness(LED_BRIGHTNESS);
    strips[i].show();
  }

  // Decalre LED pwer control as output
  pinMode(LED_PWR, OUTPUT);

  // Setup sensor inputs
  pinMode(RNG10, INPUT);

  // Give everying a moment.
  delay(100);
  cylon_start("");
  // Start the default program
  //rangerDebug_start("");
  //turnOffLEDs(); // or start with all LEDs off
}

// Simply animate the current mode.
void loop() {
  if (Serial.available() > 0) {
    mode = Serial.parseInt();
    mode > 0 ? turnOnLEDs() : turnOffLEDs();

    Serial.read();
    Serial.print("Set mode: ");
    Serial.println(mode);
  }

  switch (mode) {
    case 0:
      break;

    case 1:
      rainbow();
      break;

    case 2:
      twinkles();
      break;

    case 3:
      rangerDebug();
      break;
      
	case 4:
        cylon();
        break;
  }
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOffLEDs() {
  mode = 0;
  // Send black to make sure the LEDS are cleanly off.
  for (uint8_t stripIdx = 0; stripIdx < NUM_COLUMNS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
      strips[stripIdx].setPixelColor(i, 0x000000);
    }
    strips[stripIdx].show();
  }

  ledPower = false;
  digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.

  // Set all LED data and clock pins to HIGH so these pins can't be used as GND by the LEDs.
  digitalWrite(LED_DAT, HIGH);
  digitalWrite(LED_CLK1, HIGH);
  digitalWrite(LED_CLK2, HIGH);
  digitalWrite(LED_CLK3, HIGH);
  digitalWrite(LED_CLK4, HIGH);
  digitalWrite(LED_CLK5, HIGH);
  digitalWrite(LED_CLK6, HIGH);
  digitalWrite(LED_CLK7, HIGH);
  digitalWrite(LED_CLK8, HIGH);
  digitalWrite(LED_CLK9, HIGH);
  digitalWrite(LED_CLK10, HIGH);
}

// Cloud exposable version of turnOffLEDs()
int turnOff(String arg) {
  turnOffLEDs();
  return 1;
}

// Enables all LED by turning on the N-Channel MOSFET and connecting the LEDs to GND.
void turnOnLEDs() {
  ledPower = true;
  digitalWrite(LED_PWR, HIGH); // turn on the N-Channel transistor switch.
  delay(50); // Give the LEDs a moment to power up.
}



// =============
// = PATTERNS! =
// =============

// All pattern code and variables should be prefixed with their name and an underscore.
// There should be a `int pattern_name_start(String arg)` function for every pattern that
// is exposed in `setup()` via `Particle.function()`. The start function should simply
// set the `mode`, call `turnOnLEDs()` and `return 1`, as well as what other
// initialization the pattern may require.



// -----------
// - RAINBOW -

uint8_t rainbow_speed = 100;
uint16_t rainbow_hue = 0;

int rainbow_start(String arg) {
  mode = 1;
  turnOnLEDs();
  return 1;
}

void ShowAllColumns()
{
    // Show all strips.
    for (int col = 0; col < NUM_COLUMNS; col++) {
      strips[col].show();
    }
	
}
void rainbow() 
{
  if (!ledPower) return;

  rainbow_hue += rainbow_speed;
  uint8_t hue = rainbow_hue >> 8;

  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
      uint8_t hueOffset = (col * NUM_LEDS_PER_COLUMN) * 255 / (NUM_COLUMNS * NUM_LEDS_PER_COLUMN);
      RgbColor rgb = HsvToRgb(HsvColor(hue + hueOffset, 255, 255));
	  columns[col].SetColumnToRGB(rgb);
    }
  ShowAllColumns();
}

int cylon_start(String arg) {
  mode = 4;
  turnOnLEDs();
  return 1;
}

void cylon() {
  if (!ledPower) return;

  static uint8_t hue = 0;
  
  for (int col = 0; col < NUM_COLUMNS; col++) {
	  columns[col].SetColumnToColor(hue++);
  }
  ShowAllColumns();

}

void cylon2() 
{
  static int color = 1;
  static byte state = 0;
  static byte mask = MASK_1000;

  if (!ledPower) 
	return;

  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	columns[col].SetColumnToColor(MY_BLACK);
	columns[col].SetColumnToColorWithMask(color, mask);
    ShowAllColumns();
    delay(200);
  }

  // increment mask to rotate LED back and forth
  switch(state)
  {
    case 0: mask = MASK_0100; break;
    case 1: mask = MASK_0010; break;
    case 2: mask = MASK_0001; break;
    case 3: mask = MASK_0010; break;
    case 4: mask = MASK_0100; break;
    case 5: mask = MASK_1000; break;
    default:
      break;
  }

  if (++color >= MY_WHITE)
    color = 1;
  if (++state >= 6)
    state = 0;
}

// ------------
// - Twinkles -

uint8_t twinkles_brightness[NUM_COLUMNS * NUM_LEDS_PER_COLUMN];
bool twinkles_turningOn[NUM_COLUMNS * NUM_LEDS_PER_COLUMN];

int twinkles_start(String arg) {
  mode = 2;
  turnOnLEDs();
  for (uint8_t i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
    twinkles_brightness[i] = 0;
    twinkles_turningOn[i] = false;
  }
  return 1;
}

void twinkles() {
  for (uint8_t stripIdx = 0; stripIdx < NUM_COLUMNS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
      uint ledIdx = stripIdx * NUM_LEDS_PER_COLUMN + i;

      // animate the twinkles!
      if (twinkles_brightness[ledIdx] > 0) {
        // The pixel is turning on, count up.
        if (twinkles_turningOn[ledIdx]) {
          twinkles_brightness[ledIdx]++;
          // If max brightness, then now start dimming it.
          if (twinkles_brightness[ledIdx] == 255) {
            twinkles_turningOn[ledIdx] = false;
          }

        // The pixel is turning off, count down.
        } else if (twinkles_brightness[ledIdx] > 0) {
          twinkles_brightness[ledIdx]--;
        }
      }

      // See if we can start new twinkles now.

      // If the pixel is off.
      if (twinkles_brightness[ledIdx] == 0) {

        // if a rare random event occurred.
        if (random(100000) <= 100) {

          // Then start to turn on the pixel.
          twinkles_brightness[ledIdx] = 1;
          twinkles_turningOn[ledIdx] = true;
        }
      }
    }
  }

  for (uint8_t stripIdx = 0; stripIdx < NUM_COLUMNS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
      uint8_t brightness = twinkles_brightness[stripIdx * NUM_COLUMNS + i];
      strips[stripIdx].setPixelColor(i, brightness, brightness, brightness);
    }
    strips[stripIdx].show();
  }
}

// ----------------
// - Ranger Debug -

int rangerDebug_start(String arg) {
  mode = 3;
  turnOnLEDs();
  return 1;
}

void rangerDebug() {
  uint rng = constrain(analogRead(RNG10), 1600, 4095);
  uint8_t threshold = map(rng, 1600, 4095, 0, NUM_LEDS_PER_COLUMN);
  Serial.println(threshold);

  for (uint8_t stripIdx = 0; stripIdx < NUM_COLUMNS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS_PER_COLUMN; i++) {
      strips[stripIdx].setPixelColor(i, i < threshold ? 0xffffff : 0x000000);
    }
    strips[stripIdx].show();
  }
}
