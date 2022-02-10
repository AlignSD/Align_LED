//+-------------------------------------------------------------------------------------------------------
//
// Arduino LED Controller
//
// File:        marquee.h
//
// Description:
//
//  draws random colored stars on an LED strip for a Twinkly effect.
//
//--------------------------------------------------------------------------------------------------------

#include <FastLED.h>

#define NUM_COLORS 5
static const CRGB TwinkleColors[NUM_COLORS] =
    {
        CRGB::Red,
        CRGB::Blue,
        CRGB::Purple,
        CRGB::Green,
        CRGB::Purple,
};

void OldDrawTwinkle()
{
  FastLED.clear(false); // clear the strip but dont push out the bits

  for (int i = 0; i < NUM_LEDS / 4; i++)
  {
    g_LEDs[random(NUM_LEDS)] = TwinkleColors[random(NUM_COLORS)];
    FastLED.show(g_Brightness);
    delay(100);
  }
}

void DrawTwinkle()
{
  static int passCount = 0;
  passCount++;

  // Every time passcount hits a quarter of the LED total, we reset the strip
  if (passCount == NUM_LEDS)
  {
    passCount = 0;
    FastLED.clear(false); // clear strip but dont push out bits
  }

  g_LEDs[random(NUM_LEDS)] = TwinkleColors[random(NUM_COLORS)];
  // delay(100);
}