#include "MeterColumn.h"

#define NUM_COLUMNS 10
#define NUM_DISTANCE_SENSORS 10

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

#define RNG_1	A4
#define RNG_2	A5
#define RNG_3	B2
#define RNG_4	B3
#define RNG_5	B4
#define RNG_6	B5
#define RNG_7	A0
#define RNG_8	A1
#define RNG_9	A2
#define RNG_10	A3
#define BAT_LVL A6

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

//pattern mode
#define PAT_TURNOFF 0
#define PAT_RAINBOW 1
#define PAT_RANGER 2
#define PAT_CYLON 3
#define PAT_RANDOM 4
#define PAT_CHASE 5
#define PAT_TEST 6


uint8_t mode = PAT_RAINBOW;
bool ledPower = false;
int gBrightness = 128;
int gDistance[NUM_DISTANCE_SENSORS];
int gChaseMode = 0;
int gRainbowMode = 0;
int gRandomMode = 0;
int gBatLvl = 0;
byte gHue = 0;
int gColor = 0;
int	gThreshold = 2500;
int gDelay = 250;
int gDebug = 1;

void setup() {
  Serial.begin(9600);
  for (int i=0; i<NUM_COLUMNS; i++)
  {
	  columns[i].SetDotStars(&strips[i]);
  }

  // Expose cloud variables (up to 20)
  Particle.variable("BatLvl", gBatLvl);
  Particle.variable("Brightness", gBrightness);
  Particle.variable("Delay", gDelay);
  Particle.variable("Debug", gDebug);
  
  // Expose cloud functions.
  Particle.function("TurnOff", turnOff);

  // Every pattern should register a start funtion here.
  Particle.function("SetBright", setBrightness);
  Particle.function("SetDelay", setDelay);
  Particle.function("SetThreshold", setThreshold);
  Particle.function("SetDebug", setDebug);
  
  Particle.function("Rainbow", rainbow_start);
  Particle.function("Chase", chase_start);
  Particle.function("Ranger", ranger_start);
  Particle.function("Cylon", cylon_start);
  Particle.function("Random", random_start);
  Particle.function("Test", test_start);
  
  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].begin();
    strips[i].setBrightness(gBrightness);
  }

  // Decalre LED pwer control as output
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

  // Give everying a moment.
  delay(100);
  rainbow_start("0");
  
  // Start the default program
  //rangerDebug_start("");
  //turnOffLEDs(); // or start with all LEDs off
}

// Simply animate the current mode.
void loop() {

  readDistances();
  readBatLvl();
  
  switch (mode) 
  {
    case PAT_TURNOFF:	
	  turnOffLEDs();
      break;				// taken care of above?
    case PAT_RAINBOW:
      rainbow();	break;
    case PAT_RANGER:
      ranger();break;
	case PAT_CYLON:
      cylon2();		break;
	case PAT_RANDOM:
	  random_pat();		break;
	case PAT_CHASE:
	  chase_pat();		break;
	case PAT_TEST:
	  test_pat(); break;
	default:
	 rainbow(); break;  
  }
}

void readBatLvl()
{
	gBatLvl = analogRead(BAT_LVL);
	// Serial.print("Bat. Level: "); Serial.println(gBatLvl);
}

void readDistances()
{
	char str[200];
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
	if (gDebug > 255)
	{
		sprintf(str, "%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d",
			gDistance[0], gDistance[1], gDistance[2], gDistance[3], gDistance[4], 
			gDistance[5], gDistance[6], gDistance[7], gDistance[8], gDistance[9]);	
		Serial.println(str);
	}
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOffLEDs() 
{
  mode = PAT_TURNOFF;
  
  turnBlackLEDs();  
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

void turnBlackLEDs() 
{
  // Send black to make sure the LEDS are cleanly off.
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
  	columns[col].SetColumnToColor(MY_BLACK);
  }
}

// Cloud exposable version of turnOffLEDs()
int turnOff(String arg) 
{
  //turnOffLEDs();
  mode = PAT_TURNOFF;
  turnBlackLEDs();
  showAllColumns();
  return 1;
}

// Enables all LED by turning on the N-Channel MOSFET and connecting the LEDs to GND.
void turnOnLEDs() 
{
  ledPower = true;
  digitalWrite(LED_PWR, HIGH); // turn on the N-Channel transistor switch.
  delay(50); // Give the LEDs a moment to power up.
}

void showAllColumns()
{
    // Show all strips.
    for (int col = 0; col < NUM_COLUMNS; col++) 
	{
      strips[col].show();
    }
}

void setAllBrightness(int brightness)
{
    for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
      strips[i].setBrightness(brightness);
    }
}

// cycle gColor from 1..256 = 8-bit color hue in MyColor space
void inc_gColor()
{
	gColor++;
	if (gColor > MY_LASTCOLOR)
		gColor = 1;
}

void debugPrint() {
  if (gDebug) {
		RgbColor rgb = columns[0].ColorToRGB(gColor);
		Serial.print("rainbow color: ");
		Serial.print(gColor);
		Serial.print(" r: "); Serial.print(rgb.r);
		Serial.print(" g: "); Serial.print(rgb.g);
		Serial.print(" b: "); Serial.print(rgb.b);
		Serial.println();
	}
}

// =============
// = PATTERNS! =
// =============

// All pattern code and variables should be prefixed with their name and an underscore.
// There should be a `int pattern_name_start(String arg)` function for every pattern that
// is exposed in `setup()` via `Particle.function()`. The start function should simply
// set the `mode`, call `turnOnLEDs()` and `return 1`, as well as what other
// initialization the pattern may require.


// these are just routines that set a variable
// they don't change the current pattern.

int setBrightness(String arg) {
  
  gBrightness = arg.toInt();
  if (gBrightness < 0)
	  gBrightness = 0;
  if (gBrightness > 255)
	  gBrightness = 255;
  setAllBrightness(gBrightness);
  
  return 1;
}

int setDelay(String arg) {
  
  gDelay = arg.toInt();
  if (gDelay < 0)
	  gDelay = 0;
  if (gDelay > 10000)
	  gDelay = 10000;
  
  return 1;
}

int setThreshold(String arg) {
  
  gThreshold = arg.toInt();
  if (gThreshold < 0)
	  gThreshold = 0;
  if (gThreshold > 5000)
	  gThreshold = 5000;
  
  return 1;
}

int setDebug(String arg) {
	RgbColor rgb;
	
  	gDebug = arg.toInt();
  	if (gDebug < 0)
	  	gDebug = 0;
	
  return 1;
}


// -----------
// - RAINBOW -

uint8_t rainbow_hue = 0;

int rainbow_start(String arg) 
{
  gRainbowMode = arg.toInt();
  mode = PAT_RAINBOW;
  gColor = 1;
  gDelay = 1000;
  return 1;
}

void rainbow() 
{
  rainbow_hue++;
	switch(gRainbowMode)
	{
		default:
			for (int col = 0; col < NUM_COLUMNS; col++)
      {
        byte colHue = 255*col/NUM_COLUMNS;
        for (int i = 0; i < NUM_METERS_PER_COLUMN; i++) {
          byte meterHue = 255*i/NUM_METERS_PER_COLUMN;
          columns[col].SetMeterToHSV(i, rainbow_hue + colHue + meterHue, 255, 255);
        }
			}
		break;
	}
  showAllColumns();
  delay(25);
}

//////////////////////////////////////////////////////
int random_start(String arg) 
{
  gRandomMode = arg.toInt();
  if (gRandomMode < 0)
	  gRandomMode = 0;
  if (gRandomMode > MY_LASTCOLOR)
	  gRandomMode = MY_LASTCOLOR;
  
  mode = PAT_RANDOM;
  debugPrint();
  turnOnLEDs();
  return 1;
}

void random_pat()
{
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
		for (int meter = 0; meter < NUM_METERS_PER_COLUMN; meter++)
		{
			int color;
			if (gRandomMode == 0)
				color = random(2,255);
			else
				color = gRandomMode;
			columns[col].SetMeterToColor(meter, color);
		}
  }
	showAllColumns();
	delay(gDelay);
}

//////////////////////////////////////////////////////
int cylon_start(String arg) 
{
  gColor = 0;
  mode = PAT_CYLON;
  turnOnLEDs();
  return 1;
}

void cylon() 
{
  
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	  columns[col].SetColumnToColor(gColor);
  }
  inc_gColor();
  showAllColumns();
  delay(gDelay);
}

void cylon2() 
{
  static int color = 1;
  static byte state = 0;
  static byte mask = MASK_1000;

  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	columns[col].SetColumnToColor(MY_BLACK);
	columns[col].SetColumnToColorWithMask(color, mask);
    //delay(20);
  }
  showAllColumns();
  delay(gDelay);
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




// ----------------
// - Ranger Debug -

int ranger_start(String arg) {
  
  gColor = arg.toInt();
  if (gColor < 1)
	  gColor = 1;
  if (gColor > MY_LASTCOLOR)
	  gColor = MY_LASTCOLOR;
  mode = PAT_RANGER;
  turnBlackLEDs();
  
  return 1;
}

void ranger() {
	
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	  if (gDistance[col] > gThreshold)
		columns[col].SetColumnToColor(gColor);
	  else
		columns[col].SetColumnToColor(MY_BLACK);
  }
  showAllColumns();
}

int chase_start(String arg) {
  mode = PAT_CHASE;
  gChaseMode = arg.toInt();
  gDelay = 1000;
  return 1;  
}

void chase_pat() {
  static int curMeter = 0;
  static int curColumn = 0;
  static int curColor = 1;
  
  turnBlackLEDs();
	  switch (gChaseMode)
	  {
		  case 0:
		  	columns[curColumn++].SetColumnToColor(curColor++);
		  	if (curColor >= MY_WHITE)
			 	 curColor = 1;
		  	if (curColumn >= NUM_COLUMNS)
			  	curColumn = 0;
			break;
		case 1:
		default:
		for (int i=0; i<NUM_COLUMNS; i++)
		{
			columns[i].SetMeterToColor(curMeter, curColor);
		}
	  	if (curColor++ >= MY_WHITE)
		 	 curColor = 1;
	  	if (curMeter++ >= NUM_METERS_PER_COLUMN)
		  	curMeter = 0;
		break;
	  }
  showAllColumns();
  delay(gDelay); 
}

int test_start(String arg) {
    gColor = arg.toInt();
    if (gColor < 0)
  	  gColor = 0;
    if (gColor > MY_LASTCOLOR)
  	  gColor = MY_LASTCOLOR;
  	gDelay = 1000;
	turnBlackLEDs();
  	mode = PAT_TEST;
  	return 1;  
}

void test_one_color(int color)
{
	for (int col = 0; col < NUM_COLUMNS; col++) 
	{
		columns[col].SetColumnToColor(color);
	}
}

void test_pat() {
	test_one_color(gColor);
    showAllColumns();
	delay(gDelay);
}