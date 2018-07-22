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
	RgbColor rgb;
    switch(color)
    {
        case MY_BLACK:  rgb = RgbColor(0,0,0); break;     // set to BLACK - All Off
        case MY_WHITE:  rgb = RgbColor(255,255,255); break; // All on - WHITE
        case MY_RED:    rgb = RgbColor(255,0,0); break; // All RED
        case MY_GREEN:  rgb = RgbColor(0,255,0); break; // All GREEN
        case MY_BLUE:   rgb = RgbColor(0,0,255); break; // All BLUE
        case MY_YELLOW: rgb = RgbColor(255,255,0); break; // All YELLOW
        case MY_CYAN:   rgb = RgbColor(0,255,255); break; 
        case MY_MAGENTA: rgb = RgbColor(255,0,255); break; 
        default:
            if (color > MY_LASTCOLOR)   // bounds checking error - undefined color - turn black
            {
              rgb = RgbColor(0,0,0);
            }
            else
            {
				      rgb = HsvToRgb(HsvColor(color-1, 255, 255)); 
            }
			break;
    }
	return rgb;
}

// sets the individual LED specified by ledIndex (0..NUM_LEDS_PER_POLE) to a certain color
void MeterColumn::SetLEDToColor(byte ledIndex, int color)
{
    RgbColor rgb;
	
    if (ledIndex >= NUM_LEDS_PER_COLUMN)  // bounds check
        return;
    
    // map myColor into a FastLED hue or an RGB setting
	rgb = ColorToRGB(color);
	_pDotStars->setPixelColor(ledIndex, rgb.r, rgb.g, rgb.b);
    
}

void MeterColumn::SetLEDToRGB(byte ledIndex, byte r, byte g, byte b)
{
  if (ledIndex >= NUM_LEDS_PER_COLUMN) return; // bounds check        
  _pDotStars->setPixelColor(ledIndex, r, g, b);
}

void MeterColumn::SetLEDToRGB(byte ledIndex, RgbColor rgbColor)
{
    SetLEDToRGB(ledIndex, rgbColor.r, rgbColor.g, rgbColor.b);
}

void MeterColumn::SetLEDToHSV(byte ledIndex, byte h, byte s, byte v)
{
  SetLEDToRGB(ledIndex, HsvToRgb(HsvColor(h, s, v)));
}


// Set Meters to a solid color

void MeterColumn::SetMeterToRGBWithMask(byte meter, byte r, byte g, byte b, byte mask)
{
    int ledIndex = meter*NUM_LEDS_PER_METER;
    
    if (mask & 0b0001)
        SetLEDToRGB(ledIndex+3, r, g, b);
    if (mask & 0b0010)
        SetLEDToRGB(ledIndex+2, r, g, b);
    if (mask & 0b0100)
        SetLEDToRGB(ledIndex+1, r, g, b);
    if (mask & 0b1000)
        SetLEDToRGB(ledIndex, r, g, b);
}

// set the meter on the column to a certain color.
void MeterColumn::SetMeterToColorWithMask(byte meter, int color, byte mask)
{
    int ledIndex = meter*NUM_LEDS_PER_METER;
    
    if (mask & 0b0001)
        SetLEDToColor(ledIndex+3, color);
    if (mask & 0b0010)
        SetLEDToColor(ledIndex+2, color);
    if (mask & 0b0100)
        SetLEDToColor(ledIndex+1, color);
    if (mask & 0b1000)
        SetLEDToColor(ledIndex, color);
}

void MeterColumn::SetMeterToColor(byte meter, int color)
{
    SetMeterToColorWithMask(meter, color, MASK_1111);
}

void MeterColumn::SetMeterToHSV(byte meter, byte h, byte s, byte v)
{
    RgbColor rgb = HsvToRgb(HsvColor(h, s, v));
    SetMeterToRGBWithMask(meter, rgb.r, rgb.g, rgb.b, MASK_1111);
}

void MeterColumn::SetMeterToRGB(byte meter, byte r, byte g, byte b)
{
    SetMeterToRGBWithMask(meter, r, g, b, MASK_1111);
}

void MeterColumn::SetMeterToRGB(byte meter, RgbColor rgbColor)
{
    SetMeterToRGBWithMask(meter, rgbColor.r, rgbColor.g, rgbColor.b, MASK_1111);
}

// void MeterColumn::Set7MetersToColor(int color)
// {
// 	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
// 	{
// 	    SetMeterToColorWithMask(i, color, MASK_1111);		
// 	}
// }
// 
// void MeterColumn::Set7MetersToRGB(RgbColor rgbColor)
// {
// 	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
// 	{
// 	    SetMeterToRGBWithMask(i, rgbColor, MASK_1111);	
// 	}
// }
// 
// void MeterColumn::Set7MetersToColorWithMask(int color, byte mask)
// {
// 	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
// 	{
// 	    SetMeterToColorWithMask(i, color, mask);		
// 	}
// }
// 
// void MeterColumn::Set7MetersToRGBWithMask(RgbColor rgbColor, byte mask)
// {
// 	for (int i=2; i<NUM_METERS_PER_COLUMN; i++)
// 	{
// 	    SetMeterToRGBWithMask(i, rgbColor, mask);	
// 	}
// }

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

