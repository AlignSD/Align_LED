//+-------------------------------------------------------------------------------------------------------
//
// Arduino LED Controller
//
// File:        marquee.h
//
// Description:
//
//  draws marquee pattern to led strips
//
//--------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

void DrawMarquee()
{
  static byte j = HUE_BLUE;
  j += 4;
  byte k = j;

  // The following is roughly equivalent to fill_Rainbow(g_LEDs, NUM_LEDS, j, 8)

  CRGB c;
  for (int i = 0; i < NUM_LEDS; i++)
    g_LEDs[i] = c.setHue(k += 8);

  static int scroll = 0; // static allows value to persist across multiple calls
  scroll++;

  for (int i = scroll % 5; i < NUM_LEDS - 1; i += 5)
    g_LEDs[i] = CRGB::Black;

  delay(50);
}