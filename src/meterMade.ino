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

PatBase patterns[] = {
  patRainbow,
  patSine,
  patCylon
};

uint8_t pattern = 0;
bool gLedPower = false;
uint8_t gBrightness = 128;
int gDistance[NUM_COLUMNS];
int gSensorThreshold = 2500;
bool gSensors[NUM_COLUMNS];
int gDelay = 250;
int gDebug = 1;

int gBatLvl = 0;
int gSlrLvl = 0;

void setup() {
  Serial.begin(9600);
  
  // Setup all LED strips.
  for (int i=0; i<NUM_COLUMNS; i++) {
    columns[i].setDotStars(&strips[i]);
  }

  // Expose cloud variables. (up to 20)
  Particle.variable("batLvl", gBatLvl);
  Particle.variable("brightness", gBrightness);
  Particle.variable("delay", gDelay);
  Particle.variable("debug", gDebug);
  
  // Set Variables
  Particle.function("setBright", setBrightness);
  Particle.function("setDelay", setDelay);
  Particle.function("setThreshold", setThreshold);
  Particle.function("setDebug", setDebug);
  
  // Expose cloud functions.
  Particle.function("turnOff", turnOffFunction);
  
  // Start patterns.
  // Particle.function("Rainbow", rainbow_start);
  // Particle.function("Ranger", ranger_start);
  // Particle.function("Cylon", cylon_start);
  // Particle.function("RandomColors", randomColors_start);
  
  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].begin();
    strips[i].setBrightness(gBrightness);
  }

  // Declare LED power control as output
  pinMode(LED_PWR, OUTPUT);

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

  pinMode(BAT_LVL, INPUT);
  pinMode(SLR_LVL, INPUT);

  // Give everying a moment.
  delay(100);
  turnOff();
  
  delay(1000);
  
  for (int i=0; i<NUM_COLUMNS; i++) {
    columns[i].setDotStars(&strips[i]);
  }
  
  patRainbow.setColumns(&columns);
  patSine.setColumns(&columns);
  patCylon.setColumns(&columns);
  
  patterns[pattern].start();
}

// Simply animate the current mode.
void loop() {
  readBatLvl();
  readSlrLvl();
  
  // Nothing to do if the sculpture is powered off.
  if (!gLedPower) {
    delay(500);
    return;
  }
  
  // Get a value for every sensor.
  readDistances();
  
  
  // TEMP
  patterns[pattern].start();
}

void readBatLvl() {
  gBatLvl = analogRead(BAT_LVL);
}

void readSlrLvl() {
  gSlrLvl = analogRead(SLR_LVL);
}

void readDistances() {
  gDistance[0] = analogRead(RNG_1);
  gDistance[1] = analogRead(RNG_2);
  gDistance[2] = analogRead(RNG_3);
  gDistance[3] = analogRead(RNG_4);
  gDistance[4] = analogRead(RNG_5);
  gDistance[5] = analogRead(RNG_6);
  gDistance[6] = analogRead(RNG_7);
  gDistance[7] = analogRead(RNG_8);
  gDistance[8] = analogRead(RNG_9);
  gDistance[9] = analogRead(RNG_10);
  
  // Set the global sensor boolean values.
  for (int i = 0; i < NUM_COLUMNS; i++) {
    gSensors[i] = gDistance[i] > gSensorThreshold;
  }
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOff() {
  setAllToBlack();  
  digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.
  gLedPower = false;

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

void setAllToBlack() {
  for (int col = 0; col < NUM_COLUMNS; col++) {
    for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
      columns[col].meterRGB(i, 0,0,0);
    }
  }
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

int setDelay(String arg) {
  gDelay = arg.toInt();
  if (gDelay < 0) gDelay = 0;
  if (gDelay > 10000) gDelay = 10000;
  return 1;
}

int setThreshold(String arg) {
  gSensorThreshold = arg.toInt();
  if (gSensorThreshold < 0) gSensorThreshold = 0;
  if (gSensorThreshold > 5000) gSensorThreshold = 5000;
  return 1;
}

int setDebug(String arg) {
  gDebug = arg.toInt();
  if (gDebug < 0) gDebug = 0;
  return 1;
}