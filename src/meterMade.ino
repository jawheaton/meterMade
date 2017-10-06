#include "MeterColumn.h"

#define NUM_COLUMNS 10
#define LED_BRIGHTNESS 64
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

#define RNG_1	B0
#define RNG_2	B1
#define RNG_3	B2
#define RNG_4	B3
#define RNG_5	B4
#define RNG_6	B5
#define RNG_7	A0
#define RNG_8	A1
#define RNG_9	A2
#define RNG_10	A3


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

//pattern mode
#define PAT_TURNOFF 0
#define PAT_RAINBOW 1
#define PAT_BRIGHTNESS 2
#define PAT_RANGERDEBUG 3
#define PAT_CYLON 4
#define PAT_RANDOM 5


uint8_t mode = PAT_RAINBOW;
bool ledPower = false;
int gBrightness = LED_BRIGHTNESS;
int gDistance[NUM_DISTANCE_SENSORS];


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
  Particle.function("Brightness", brightness_start);
  Particle.function("RangerDebug", rangerDebug_start);
  Particle.function("Cylon", cylon_start);
  Particle.function("Random", random_start);

  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].begin();
    strips[i].setBrightness(gBrightness);
    strips[i].show();
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

  // Give everying a moment.
  delay(100);
  rainbow_start("");
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

  readDistances();
  
  switch (mode) 
  {
    case PAT_TURNOFF:	
      break;				// taken care of above?
    case PAT_RAINBOW:
      rainbow();	break;
    case PAT_BRIGHTNESS:
      brightness();	break;
    case PAT_RANGERDEBUG:
      rangerDebug();break;
	case PAT_CYLON:
      cylon2();		break;
	case PAT_RANDOM:
	  random_pat();		break;
  }
}

void readDistances()
{
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
	for (int i=0; i<10; i++)
	{
	    Serial.print(gDistance[i]); Serial.print(" ");
	}
	Serial.println();
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
      strips[i].begin();
      strips[i].setBrightness(brightness);
      strips[i].show();
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



// -----------
// - RAINBOW -

uint8_t rainbow_speed = 100;
uint16_t rainbow_hue = 0;

int rainbow_start(String arg) 
{
  mode = PAT_RAINBOW;
  turnOnLEDs();
  for (uint8_t i = 0; i < NUM_COLUMNS; i++) {
    strips[i].setBrightness(LED_BRIGHTNESS);
  }
  return 1;
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
  showAllColumns();
}

//////////////////////////////////////////////////////
int random_start(String arg) 
{
  mode = PAT_RANDOM;
  turnOnLEDs();
  return 1;
}

void random_pat()
{
    for (int col = 0; col < NUM_COLUMNS; col++) 
    {
		for (int meter = 0; meter < NUM_METERS_PER_COLUMN; meter++)
		{
			int color = random(1, 256);
			columns[col].SetMeterToColor(meter, color);
		}
    }
	showAllColumns();
	delay(500);
}

//////////////////////////////////////////////////////
int cylon_start(String arg) 
{
  mode = PAT_CYLON;
  turnOnLEDs();
  return 1;
}

void cylon() 
{
  if (!ledPower) return;

  static uint8_t hue = 0;
  
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	  columns[col].SetColumnToColor(hue++);
  }
  showAllColumns();
  delay(500);
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
    //delay(20);
  }
  showAllColumns();
  delay(250);
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
// - Brightness Test -

int brightness_start(String arg) {
  int newBrightness;
  
  newBrightness = arg.toInt();

  mode = PAT_BRIGHTNESS;
  turnOnLEDs();
  gBrightness = newBrightness;
  if (gBrightness < 0)
	  gBrightness = 0;
  if (gBrightness > 255)
	  gBrightness = 255;
  turnBlackLEDs();
  
  return 1;
}

void brightness() 
{
	static int color = MY_RED;
	static int myBrightness = 0;
	
	myBrightness += 1;
	if (myBrightness >= gBrightness)
	{
		delay(2000);	// pause at maximum brightness
		myBrightness = 0;
		switch (color)
		{
			case MY_RED: color = MY_BLUE; break;
			case MY_BLUE: color = MY_GREEN; break;
			case MY_GREEN: color = MY_WHITE; break;
			case MY_WHITE: color = MY_RED; break;
			default: break;
		}
	}
	// step through colors and brightnesses
    for (int col = 0; col < NUM_COLUMNS; col++) 
    {
		for (int meter = 0; meter < NUM_METERS_PER_COLUMN; meter++)
		{
			columns[col].SetMeterToColor(meter, color);
		}
    }
	setAllBrightness(myBrightness);
	showAllColumns();
}


// ----------------
// - Ranger Debug -

int rangerDebug_start(String arg) {
  mode = PAT_RANGERDEBUG;
  turnOnLEDs();
  return 1;
}

void rangerDebug() {
	
  for (int col = 0; col < NUM_COLUMNS; col++) 
  {
	for (int meter = 0; meter < NUM_METERS_PER_COLUMN; meter++)
	{
		int color;
		if (gDistance[2] > 2200)
			color = map(gDistance[2], 2000, 4000, 0, 128);
		else
			color = MY_BLACK;
		columns[col].SetMeterToColor(meter, color);
	}
  }
  showAllColumns();
  delay(20);
  
}
