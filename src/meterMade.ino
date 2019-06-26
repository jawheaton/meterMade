#include "MeterColumn.h"

#define BAT_LVL A6
#define SLR_LVL A7
#define LED_PWR D7
#define LED_DAT D6

#define LED_CLK1 C0
#define LED_CLK2 C1
#define LED_CLK3 C2
#define LED_CLK4 C3
#define LED_CLK5 C4
#define LED_CLK6 C5
#define LED_CLK7 D0
#define LED_CLK8 D1
#define LED_CLK9 D2
#define LED_CLK10 D3

#define LOW_BAT 2450
#define DUSK 2100

Adafruit_DotStar strips[] = {
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK1, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK2, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK3, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK4, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK5, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK6, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK7, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK8, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK9, DOTSTAR_BGR),
    Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK10, DOTSTAR_BGR)};

MeterColumn columns[NUM_COLUMNS];

// Instantiate patterns
#include "Rainbow.h"
Rainbow patRainbow;

#include "Sine.h"
Sine patSine;

#include "Cylon.h"
Cylon patCylon;

#include "Random.h"
Random patRandom;

#include "Fire.h"
Fire patFire;

#define NUM_PATTERNS 5

PatBase *patterns[NUM_PATTERNS];

// The index of the current pattern.
int gPattern = 0;

// The number of patterns. This is exposed to the particle API so that you know
// that range when setting a pattern.
int gNumPatterns = NUM_PATTERNS;

// True when the LEDs are on and animating.
bool gLedPower = false;

// Brightness of the LEDS.
int gBrightness = 128;

// The analogRead value of the battery.  ~2650 seems to be full. ~2350 seems to
// empty.
int gBatLvl = 0;

// The analogRead value of the solar panel. Day time this should be the same as
// the battery level. Nighttime, it should be near zero.
int gSlrLvl = 0;

// Save the current hour in order to observe when the hour changes and run the
// hourly tasks.
int gCurrentHour = 0;

void setup() {
  Serial.begin(9600);
  Time.zone(-7);

  setupParticle();
  setupLeds();
  setupPatterns();

  // Give everying a moment.
  delay(100);
  turnOff();
}

void setupParticle() {
  Particle.variable("power", gLedPower);
  Particle.variable("brightness", gBrightness);
  Particle.variable("pattern", gPattern);
  Particle.variable("numPatterns", gNumPatterns);
  Particle.variable("batLvl", gBatLvl);
  Particle.variable("slrLvl", gSlrLvl);

  Particle.function("setBright", setBrightness);
  Particle.function("turnOff", turnOffFunction);
  Particle.function("startPattern", startPattern);
}

void setupPins() {
  pinMode(LED_PWR, OUTPUT);
  pinMode(BAT_LVL, INPUT);
  pinMode(SLR_LVL, INPUT);
}

void setupLeds() {
  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].begin();
    strips[i].setBrightness(gBrightness);
  }

  // Setup all LED strips.
  for (int i = 0; i < NUM_COLUMNS; i++) {
    columns[i].setDotStars(&strips[i]);
  }
}

void setupPatterns() {
  patterns[0] = &patRainbow;
  patterns[1] = &patCylon;
  patterns[2] = &patRandom;
  patterns[3] = &patSine;
  patterns[4] = &patFire;

  for (int i = 0; i < NUM_PATTERNS; i++) {
    patterns[i]->setColumns(columns);
  }
}

// Simply animate the current mode.
void loop() {
  gBatLvl = analogRead(BAT_LVL);
  gSlrLvl = analogRead(SLR_LVL);

  checkTime();

  // Only run the main loop if we are powered on.
  if (gLedPower) {
    // Animate the pattern.
    patterns[gPattern]->loop();
  }

  // Turn on when it should.
  if (shouldBePoweredOn() && !gLedPower) {
    turnOn();
    startPattern(String(random(NUM_PATTERNS)));
  }

  // Turn off when it should.
  if (!shouldBePoweredOn() && gLedPower) {
    turnOff();
  }

  // Low power mode.
  if (shouldBePoweredOn() && gBatLvl < LOW_BAT) {
    patRandom.lowPower = true;
    if (gPattern != 2) {
      gPattern = 2;
      patRandom.start();
    }
  }
}

// Returns true when the led should be lit up.
bool shouldBePoweredOn() {
  // Nightime, but not too late.
  if (gSlrLvl < DUSK && (gCurrentHour < 1 || gCurrentHour > 12)) {
    return true;
  }

  // Daytime.
  return false;
}

// On the hour, switch the pattern.
void checkTime() {
  if (Time.hour() != gCurrentHour) {
    gCurrentHour = Time.hour();

    if (shouldBePoweredOn()) {
      startPattern(String(random(NUM_PATTERNS)));
    }
  }
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOff() {
  for (int col = 0; col < NUM_COLUMNS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
      columns[col].meterRGB(i, 0, 0, 0);
    }
  }

  // Yeah, we have to do this twice... Gremlins.
  showAllColumns();
  showAllColumns();

  // digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.
  gLedPower = false;

  // Set all LED data and clock pins to HIGH so these pins can't be used as
  // GND by the LEDs. digitalWrite(LED_DAT, HIGH); digitalWrite(LED_CLK1,
  // HIGH); digitalWrite(LED_CLK2, HIGH); digitalWrite(LED_CLK3, HIGH);
  // digitalWrite(LED_CLK4, HIGH);
  // digitalWrite(LED_CLK5, HIGH);
  // digitalWrite(LED_CLK6, HIGH);
  // digitalWrite(LED_CLK7, HIGH);
  // digitalWrite(LED_CLK8, HIGH);
  // digitalWrite(LED_CLK9, HIGH);
  // digitalWrite(LED_CLK10, HIGH);
}

int startPattern(String arg) {
  uint8_t i = arg.toInt();
  if (i >= 0 && i < NUM_PATTERNS) {
    turnOn();
    gPattern = i;
    patterns[gPattern]->start();
    return 1;
  } else {
    return -1;
  }
}

void setAllToBlack() {
  for (int col = 0; col < NUM_COLUMNS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
      columns[col].meterRGB(i, 0, 0, 0);
    }
  }
  showAllColumns();
}

// Cloud exposable version of turnOff()
int turnOffFunction(String arg) {
  turnOff();
  return 1;
}

// Enables all LED by turning on the N-Channel MOSFET and connecting the LEDs
// to GND.
void turnOn() {
  gLedPower = true;
  digitalWrite(LED_PWR, HIGH);  // turn on the N-Channel transistor switch.
  delay(50);                    // Give the LEDs a moment to power up.
}

void showAllColumns() {
  // Show all strips.
  for (int col = 0; col < NUM_COLUMNS; col++) {
    strips[col].show();
  }
}

// these are just routines that set a variable
// they don't change the current pattern.

int setBrightness(String arg) {
  gBrightness = arg.toInt();
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].setBrightness(gBrightness);
  }
  return 1;
}
