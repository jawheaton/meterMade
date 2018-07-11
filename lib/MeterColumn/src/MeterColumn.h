#pragma once
/*
    Meter Made Project 
    (c) 2017 Built by Big
    Written by Jim Wheaton
 
    MeterColumn - representation of a single column of the Meter sculpture
        Each Column has 9 meters (7 singles and 1 double meter at the top).
        Each Meter has 4 LEDs
*/

// This will load the definition for common Particle variable types
#include "Particle.h"
#include "hsv.h"
#include "dotstar.h"

#define NUM_LEDS_PER_METER 4
#define NUM_METERS_PER_COLUMN 9
#define NUM_LEDS_PER_COLUMN NUM_LEDS_PER_METER * NUM_METERS_PER_COLUMN

#define MASK_0000   0x00
#define MASK_0001   0x01
#define MASK_0010   0x02
#define MASK_0100   0x04
#define MASK_1000   0x08
#define MASK_0110   0x06
#define MASK_1001   0x09
#define MASK_1111   0x0f


// myHue is an int which is interpreted as follows:
//  0 = Black (all off RGB)
//  1..256 is the FastLED library hue (0..255)
//  257=WHITE, 258=RED, 259=GREEN, 260=BLUE, etc... See #defines below
#define MY_BLACK 0
#define MY_WHITE 257
#define MY_RED 258
#define MY_GREEN 259
#define MY_BLUE 260
#define MY_YELLOW 261
#define MY_CYAN 262
#define MY_MAGENTA 263
#define MY_LASTCOLOR MY_MAGENTA


class MeterColumn {
public:
    MeterColumn();
    MeterColumn(Adafruit_DotStar* pDotStars);
    void SetDotStars(Adafruit_DotStar* pDotStars);
    
	
    // sets the individual LED specified by ledIndex (0..NUM_LEDS_PER_COLUMN) to a certain color
    void SetLEDToColor(byte ledIndex, int color);
    void SetLEDToRGB(byte ledIndex, RgbColor rgbColor);
    
    // set the meter on the pole to a certain color
    // mode should be METER_COLOR_MODE_ALL normally
    void SetMeterToColorWithMask(byte meter, int color, byte mask);
    void SetMeterToRGBWithMask(byte meter, RgbColor rgbColor, byte mask);
    void SetMeterToColor(byte meter, int color);
    void SetMeterToRGB(byte meter, RgbColor rgbColor);

	// set the bottom 7 meters to the same color
    void Set7MetersToColorWithMask(int color, byte mask);
    void Set7MetersToRGBWithMask(RgbColor rgbColor, byte mask);
    void Set7MetersToColor(int color);
    void Set7MetersToRGB(RgbColor rgbColor);

    // Set entire column to color
    void SetColumnToColorWithMask(int color, byte mask);
    void SetColumnToRGBWitMask(RgbColor rbgColor, byte mask);
    void SetColumnToColor(int color);
    void SetColumnToRGB(RgbColor rgbColor);
    
    // Test Routines
    void TestNextLED();
	RgbColor ColorToRGB(int color);
    
    
private:
    Adafruit_DotStar* _pDotStars;        // pointer to array of NUM_LEDS_PER_COLUMN dotstar LEDS
};

