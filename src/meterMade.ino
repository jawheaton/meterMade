#include "MeterColumn.h"

#define BAT_LVL A6
#define SLR_LVL A7
#define LED_PWR   D7
#define LED_DAT   D6

#define LED_CLK1  C0
#define LED_CLK2  C1
#define LED_CLK3  C2
#define LED_CLK4  C3
#define LED_CLK5  C4
#define LED_CLK6  C5
#define LED_CLK7  D0
#define LED_CLK8  D1
#define LED_CLK9  D2
#define LED_CLK10 D3

#define RNG_1   A5
#define RNG_2   A4
#define RNG_3   A3
#define RNG_4   A2
#define RNG_5   A1
#define RNG_6   A0
#define RNG_7   B5
#define RNG_8   B4
#define RNG_9   B3
#define RNG_10  B2

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
  Adafruit_DotStar(NUM_LEDS_PER_COLUMN, LED_DAT, LED_CLK10, DOTSTAR_BGR)
};

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

#include "ChaseUp.h"
ChaseUp patChaseUp;

#include "ChaseAround.h"
ChaseUp patChaseAround;

#define NUM_PATTERNS 6

PatBase* patterns[NUM_PATTERNS];

// The index of the current pattern.
int gPattern = 0;

// The number of patterns. This is exposed to the particle API so that you know that range
// when setting a pattern.
int gNumPatterns = NUM_PATTERNS;

// Force a delay between each frame, in addtion tp what is in that pattern.
int gDelay = 0;

// True when the LEDs are on and animating.
bool gLedPower = false;

// Brightness of the LEDS.
int gBrightness = 128;

// An boolean for every sensor. True of that sensor value is > gSensorThreshold
bool gSensors[NUM_COLUMNS];

// Previous sensor state, used for interrupting a delay to animate a column.
bool gLastSensors[NUM_COLUMNS];

// True if any sensor was just activated.
bool gSensorsChanged = false;

// Raw sensor readings.
int gDistance[NUM_COLUMNS];

// The value that sensor must be to activated.
int gSensorThreshold = 2500;

// The analogRead value of the battery.  ~2650 seems to be full. ~2350 seems to empty.
int gBatLvl = 0;

// The analogRead value of the solar panel. Day time this should be the same as the
// battery level. Nighttime, it should be near zero.
int gSlrLvl = 0;

// Save the current hour in order to observe when the hour changes and run the hourly tasks.
int gCurrentHour = 0;

void setup() {
  Serial.begin(9600);

  setupParticle();
  setupLeds();
  setupSensors();
  setupPatterns();
  setupVariables();

  // Give everying a moment.
  delay(100);
  turnOff();
}

void setupVariables()
{
  for (int i=0; i<NUM_COLUMNS; i++)
  {
    gSensors[i] = gLastSensors[i] = false;
  }
}

void setupParticle() {
  // Variables getters.
  Particle.variable("power", gLedPower);
  Particle.variable("brightness", gBrightness);
  Particle.variable("pattern", gPattern);
  Particle.variable("numPatterns", gNumPatterns);
  Particle.variable("threshold", gSensorThreshold);
  Particle.variable("batLvl", gBatLvl);
  Particle.variable("slrLvl", gSlrLvl);
  
  // Variables setters.
  Particle.function("setBright", setBrightness);
  Particle.function("setThreshold", setThreshold);
  
  // Functions.
  Particle.function("turnOff", turnOffFunction);
  
  // Start patterns.
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
  for (int i=0; i<NUM_COLUMNS; i++) {
    columns[i].setDotStars(&strips[i]);
  }
}

void setupSensors() {
  // Setup sensor inputs
  pinMode(RNG_1, INPUT);
  pinMode(RNG_2, INPUT);
  pinMode(RNG_3, INPUT);
  pinMode(RNG_4, INPUT);
  pinMode(RNG_5, INPUT);
  pinMode(RNG_6, INPUT);
  pinMode(RNG_7, INPUT);
  pinMode(RNG_8, INPUT);
  pinMode(RNG_9, INPUT);
  pinMode(RNG_10, INPUT);
}

void setupPatterns() {
  patterns[0] = &patRainbow;
  patterns[1] = &patSine;
  patterns[2] = &patCylon;
  patterns[3] = &patRandom;
  patterns[4] = &patChaseUp;
  patterns[5] = &patChaseAround;

  for (int i = 0; i < NUM_PATTERNS; i++) {
    patterns[i]->setColumns(columns);
  }
}

// Simply animate the current mode.
void loop() {
  readBatLvl();
  readSlrLvl();
  checkTime();
  
  // Only run the main loop if we are powered on.
  if (gLedPower) {
    // Get a value for every sensor.
    readDistances();
    
    // Send the currently triggered sensors to the pattern.
    patterns[gPattern]->setSensors(gSensors);
    
    // Animate the pattern.
    patterns[gPattern]->loop();

    // Global delay (to spped up or slow down any pattern
    // distances are read again and delay stops if gSensorsStateChanged
    if ((gDelay < 10000) && (gDelay > 0)) {
      delayAndReadDistances(gDelay);
    }
  }
}

// On the hour, see if we should turn on or off based on light availabily.
// If it's on, change the pattern.
void checkTime() {
  if (hour() != gCurrentHour) {
    gCurrentHour = hour();
    
    // Nighttime: If off, turn on. Set a random pattern.
    if (gSlrLvl < 1000) {
      if (!gLedPower) turnOn();
      startPattern(String(random(NUM_PATTERNS)));
    }
    
    // Daytime: Turn off.
    else if (gLedPower) {
      turnOff();
    }
  }
}

void readBatLvl() {
  gBatLvl = analogRead(BAT_LVL);
}

void readSlrLvl() {
  gSlrLvl = analogRead(SLR_LVL);
}

void readDistances() {
  static const float oldScale = 0.2;
  static const float newScale = 1.0 - oldScale;
  
  // set history of sensor values to detect any change
  for (int i=0; i<NUM_COLUMNS; i++)
  {
    gLastSensors[i] = gSensors[i];  // copy previous state
  }
  gSensorsChanged = false;

  // get new values and scale them
  gDistance[0] = oldScale * gDistance[0] + newScale * analogRead(RNG_1);
  gDistance[1] = oldScale * gDistance[1] + newScale * analogRead(RNG_2);
  gDistance[2] = oldScale * gDistance[2] + newScale * analogRead(RNG_3);
  gDistance[3] = oldScale * gDistance[3] + newScale * analogRead(RNG_4);
  gDistance[4] = oldScale * gDistance[4] + newScale * analogRead(RNG_5);
  gDistance[5] = oldScale * gDistance[5] + newScale * analogRead(RNG_6);
  gDistance[6] = oldScale * gDistance[6] + newScale * analogRead(RNG_7);
  gDistance[7] = oldScale * gDistance[7] + newScale * analogRead(RNG_8);
  gDistance[8] = oldScale * gDistance[8] + newScale * analogRead(RNG_9);
  gDistance[9] = oldScale * gDistance[9] + newScale * analogRead(RNG_10);
  
   // Set the global sensor boolean values.
  for (int i = 0; i < NUM_COLUMNS; i++) {
    gSensors[i] = gDistance[i] > gSensorThreshold;
    if (gSensors[i] != gLastSensors[i])
      gSensorsChanged = true;
  }
}

void delayAndReadDistances(int myDelay)
{
  long startTime = millis();
  if (myDelay < 0)
    return;
  while ((millis() - startTime) < (long)myDelay)
  {
    readDistances();    // update distance values and sensor (trigger) state
    if (gSensorsChanged)
      break;            // if movement detected break out of delay loop to let pattern adapt.
    delay(1);
  }
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOff() {
  for (int col = 0; col < NUM_COLUMNS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
      columns[col].meterRGB(i, 0,0,0);
    }
  }
  showAllColumns();
  
  // digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.
  gLedPower = false;

  // Set all LED data and clock pins to HIGH so these pins can't be used as GND by the LEDs.
  // digitalWrite(LED_DAT, HIGH);
  // digitalWrite(LED_CLK1, HIGH);
  // digitalWrite(LED_CLK2, HIGH);
  // digitalWrite(LED_CLK3, HIGH);
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
    // set any pattern specific global values at pattern start time(e.g. gDelay, gBrightness, etc...
    switch (gPattern)
    {
      case 3: // random 
        gDelay = 500; break;
      case 4: // Chase up
        gDelay = 1000; break;
      case 5: // Chase Around
        gDelay = 500; break;
      default:
        gDelay = 0;
        break;
    }
    patterns[gPattern]->start();
    return 1;
  } else {
    return -1;
  }
}

void setAllToBlack() {
  for (int col = 0; col < NUM_COLUMNS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
      columns[col].meterRGB(i, 0,0,0);
    }
  }
  showAllColumns();
}

// Cloud exposable version of turnOff()
int turnOffFunction(String arg) {
  turnOff();
  return 1;
}

// Enables all LED by turning on the N-Channel MOSFET and connecting the LEDs to GND.
void turnOn() {
  gLedPower = true;
  digitalWrite(LED_PWR, HIGH); // turn on the N-Channel transistor switch.
  delay(50); // Give the LEDs a moment to power up.
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

int setThreshold(String arg) {
  gSensorThreshold = arg.toInt();
  if (gSensorThreshold < 0) gSensorThreshold = 0;
  if (gSensorThreshold > 5000) gSensorThreshold = 5000;
  return 1;
}