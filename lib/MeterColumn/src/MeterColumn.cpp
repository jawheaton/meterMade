/*
 MeterMade Project
 (c) 2017 Built by Big
 Written by Jim Wheaton
 
 MeterColumn - representation of a single column of the Meter sculpture
 Each Column has 9 meters (7 singles and 1 double meter at the top).
 Each Meter has 4 LEDs
 */

#include "MeterColumn.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif


/**************************** Contruct MeterColumn  ****************************/
MeterColumn::MeterColumn(void)
{
    _pDotStars = NULL;
}

MeterColumn::MeterColumn(Adafruit_DotStar* pDotStars)
{
    _pDotStars = pDotStars;
}

/***********************************/
void MeterColumn::SetDotStars(Adafruit_DotStar* pDotStars)
{
    _pDotStars = pDotStars;
}

RgbColor MeterColumn::ColorToRGB(int color)
{
	byte hue = (byte)color;
    RgbColor rgb = HsvToRgb(HsvColor(hue, 255, 255));
	return rgb;
}

// sets the individual LED specified by ledIndex (0..NUM_LEDS_PER_POLE) to a certain color
void MeterColumn::SetLEDToColor(byte ledIndex, int color)
{
    byte hue = 0;	// when not black = 0, or white = 257, we used a byte value for hue mapping (0..255)
    
    if (ledIndex >= NUM_LEDS_PER_COLUMN)  // bounds check
        return;
    
    // map myColor into a FastLED hue or an RGB setting
    switch(color)
    {
        case MY_BLACK:  _pDotStars->setPixelColor(ledIndex, 0, 0, 0);  break;     // set to BLACK - All Off
        case MY_WHITE:  _pDotStars->setPixelColor(ledIndex, 255,255,255); break; // All on - WHITE
        case MY_RED:    _pDotStars->setPixelColor(ledIndex, 255,0,0); break; // All RED
        case MY_GREEN:  _pDotStars->setPixelColor(ledIndex, 0,255,0); break; // All GREEN
        case MY_BLUE:   _pDotStars->setPixelColor(ledIndex, 0,0,255); break; // All BLUE
        default:
            if (color > MY_LASTCOLOR)   // bounds checking error - undefined color - turn black
            {
                _pDotStars->setPixelColor(ledIndex, 0, 0, 0);
            }
            else
            {
                hue = color-1;	// color must be 1..256 at this point
			    RgbColor rgb = HsvToRgb(HsvColor(hue, 255, 255));
		        _pDotStars->setPixelColor(ledIndex, rgb.r, rgb.b, rgb.g);
            }
            break;
    }
    
}

void MeterColumn::SetLEDToRGB(byte ledIndex, RgbColor rgbColor)
{
    if (ledIndex >= NUM_LEDS_PER_COLUMN)  // bounds check
        return;
    _pDotStars->setPixelColor(ledIndex, rgbColor.r, rgbColor.b, rgbColor.g);
}

// set the meter on the column to a certain color.
void MeterColumn::SetMeterToColorWithMask(byte meter, int color, byte mask)
{
    int ledIndex = meter*NUM_LEDS_PER_METER;
    
    if (mask & 0x1)
        SetLEDToColor(ledIndex+3, color);
    if (mask & 0x2)
        SetLEDToColor(ledIndex+2, color);
    if (mask & 0x4)
        SetLEDToColor(ledIndex+1, color);
    if (mask & 0x8)
        SetLEDToColor(ledIndex, color);
}

void MeterColumn::SetMeterToRGBWithMask(byte meter, RgbColor rgbColor, byte mask)
{
    int ledIndex = meter*NUM_LEDS_PER_METER;
    
    if (mask & 0x1)
        SetLEDToRGB(ledIndex+3, rgbColor);
    if (mask & 0x2)
        SetLEDToRGB(ledIndex+2, rgbColor);
    if (mask & 0x4)
        SetLEDToRGB(ledIndex+1, rgbColor);
    if (mask & 0x8)
        SetLEDToRGB(ledIndex, rgbColor);

}
void MeterColumn::SetMeterToColor(byte meter, int color)
{
    SetMeterToColorWithMask(meter, color, MASK_1111);
}

void MeterColumn::SetMeterToRGB(byte meter, RgbColor rgbColor)
{
    SetMeterToRGBWithMask(meter, rgbColor, MASK_1111);
}

void MeterColumn::Set7MetersToColor(int color)
{
	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
	{
	    SetMeterToColorWithMask(i, color, MASK_1111);		
	}
}

void MeterColumn::Set7MetersToRGB(RgbColor rgbColor)
{
	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
	{
	    SetMeterToRGBWithMask(i, rgbColor, MASK_1111);	
	}
}

void MeterColumn::Set7MetersToColorWithMask(int color, byte mask)
{
	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
	{
	    SetMeterToColorWithMask(i, color, mask);		
	}
}

void MeterColumn::Set7MetersToRGBWithMask(RgbColor rgbColor, byte mask)
{
	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
	{
	    SetMeterToRGBWithMask(i, rgbColor, mask);	
	}
}
// Set entire column to color
void MeterColumn::SetColumnToColorWithMask(int color, byte mask)
{
    for (int i=0; i<NUM_METERS_PER_COLUMN; i++)
    {
        SetMeterToColorWithMask(i, color, mask);
    }
}

void MeterColumn::SetColumnToRGBWitMask(RgbColor rbgColor, byte mask)
{
    for (int i=0; i<NUM_METERS_PER_COLUMN; i++)
    {
        SetMeterToRGBWithMask(i, rbgColor, mask);
    }
}

void MeterColumn::SetColumnToColor(int color)
{
    for (int i=0; i<NUM_METERS_PER_COLUMN; i++)
    {
        SetMeterToColorWithMask(i, color, MASK_1111);
    }

}

void MeterColumn::SetColumnToRGB(RgbColor rbgColor)
{
    for (int i=0; i<NUM_METERS_PER_COLUMN; i++)
    {
        SetMeterToRGBWithMask(i, rbgColor, MASK_1111);
    }
    
}


// Test Routines
void MeterColumn::TestNextLED()
{
    static int ledIndex = 0;
    
    SetColumnToColor(MY_BLACK);
    SetLEDToColor(ledIndex++, MY_WHITE);
    if (ledIndex >= NUM_LEDS_PER_COLUMN)
        ledIndex = 0;
}

