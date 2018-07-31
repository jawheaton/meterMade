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

#define NUM_COLUMNS 10
#define NUM_LEDS_PER_METER 4
#define NUM_METERS_PER_COLUMN 9
#define NUM_LEDS_PER_COLUMN NUM_LEDS_PER_METER * NUM_METERS_PER_COLUMN

// Some shortcut hue values.
#define HUE_RED 0
#define HUE_GREEN 85
#define HUE_BLUE 170
#define HUE_YELLOW 42
#define HUE_CYAN 127
#define HUE_MAGENTA 212

class MeterColumn {
public:
    MeterColumn();
    MeterColumn(Adafruit_DotStar* pDotStars);
    void setDotStars(Adafruit_DotStar* pDotStars);
    Adafruit_DotStar* leds;        // pointer to array of NUM_LEDS_PER_COLUMN dotstar LEDS
    
    void ledRGB(byte ledIndex, byte r, byte g, byte b);
    void ledHSV(byte ledIndex, byte h, byte s, byte v);
    void meterRGB(byte meterIndex, byte r, byte g, byte b, byte mask = 0b1111);
    void meterHSV(byte meterIndex, byte h, byte s, byte v, byte mask = 0b1111);
};

