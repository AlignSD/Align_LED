//+-------------------------------------------------------------------------------------------------------
//
// Arduino LED Controller
//
// File:        marquee.h
//
// Description:
//
//  draws comet pattern to led strips
//
//--------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

extern CRGB g_LEDs[];

void DrawComet()
{
  const byte fadeAmt = 64;       // fade half the color out pixel if its chosen to be faded
  const int cometSize = 5;       // size of the comet in pixels
  const int deltaHue = 4;        // how far to step the cycling hue each draw
  const double cometSpeed = 0.5; // how far to advance the comet per frame
  static byte hue = HUE_RED;     // Current color
  static int iDirection = 1;     // Current direction (-1 or +1)
  static double iPos = 0;        // Current comet position on strip

  hue += deltaHue;                 // Update the comet color
  iPos += iDirection * cometSpeed; // Update the comet position

  // Flip comet direction when we hit end of the LED Strip
  if (iPos == (NUM_LEDS - cometSize) || iPos == 0)
    iDirection *= -1;

  // Draw the comet at its current position
  for (int i = 0; i < cometSize; i++)
    g_LEDs[(int)iPos + i].setHue(hue);

  // Fade the LEDs one step
  for (int j = 0; j < NUM_LEDS; j++)
    if (random(2) == 1)
      g_LEDs[j] = g_LEDs[j].fadeToBlackBy(fadeAmt);

  delay(20); // 25 * 40 = 1000ms or 1 second
}