//+-------------------------------------------------------------------------------------------------------
//
// Arduino LED Controller
//
// File:        Main.cpp
//
// Description:
//
//  Controller for WS2812B LED light strips and OLED diplay
//
//--------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#define OLED_CLOCK 15 // pins for the oled display
#define OLED_DATA 4   //
#define OLED_RESET 16

#define NUM_LEDS 80 // Number of LEDS in light strip
#define LED_PIN 5

CRGB g_LEDs[NUM_LEDS] = {0}; // Frame buffer for fastLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 255;  // 0-255 brightness scale
int g_PowerLimit = 3000; // 3000mW power limit

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))            // counts the number of elements in a staticly assigned array.
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000 / x) // every n function only runs code at certain time intervals

// Fractional color
//
// Returns a fraction of a color; abstracts the fadeToBlack out to this function in case we
// want to improve the color correction all in one location at a later date

CRGB ColorFraction(CRGB colorIn, float fraction)
{
  fraction = min(1.0f, fraction);
  return CRGB(colorIn).fadeToBlackBy(255 * (1.0f - fraction));
}

// FramesPerSecond
//
// Tracks a weighted average in order to smooth out the balues that it is given. Cputes the FPS as the simple reciprocal
// of the amount of time taken specifed by the caller. so 1/3 of a second is 3 fps. it takes about 10 frames to stabilize

double FramesPerSecond(double seconds)
{
  static double framesPerSecond;
  framesPerSecond = (framesPerSecond * .9) + (1.0 / seconds * .1); // keeps 90% of the old value and takes 10% of the new value
  return framesPerSecond;
}

void DrawPixels(float fPos, float count, CRGB color)
{
  // Calculate how much the first pixel will hold
  float availFirstPixel = 1.0f - (fPos - (long)(fPos));
  float amtFirstPixel = min(availFirstPixel, count);
  float remaining = min(count, FastLED.size() - fPos);
  int iPos = fPos;

  // Blend (add) in the color of the first partial pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos++] += ColorFraction(color, amtFirstPixel);
    remaining -= amtFirstPixel;
  }

  // now draw any full pixels in the middle

  while (remaining > 1.0f)
  {
    FastLED.leds()[iPos++] += color;
    remaining--;
  }

  // draw tail pixel, up to a single full pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos] += ColorFraction(color, remaining);
  }
}

void DrawMarqueeComparison()
{
  static float scroll = 0.0f;
  scroll += 0.1f;
  if (scroll > 5.0)
    scroll -= 5.0;

  for (float i = scroll; i < NUM_LEDS / 2 - 1; i += 5)
  {
    DrawPixels(i, 3, CRGB::Green);
    DrawPixels(NUM_LEDS - 1 - (int)i, 3, CRGB::Red);
  }
}

void set_max_power_indicator_LED(uint8_t);

#include <marquee.h>
#include <twinkle.h>
#include <comet.h>
#include <bounce.h>
#include "fire.h"

void setup() // put your setup code here, to run once:
{
  // Pin Initialization

  pinMode(LED_BUILTIN, OUTPUT); // sets pin-mode to output for builtin LED
  pinMode(LED_PIN, OUTPUT);     // pin mode for FastLED strips

  // Serial Initialization

  Serial.begin(115200); // For later debugging
  while (!Serial)
  {
  }
  Serial.println("ESP32 Startup");

  // Handle OLED Drawing

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // Descent is a negative number so we add it to total

  // LED Strip Initialization

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS); // add out LED light strips
  FastLED.setBrightness(255);
  set_max_power_indicator_LED(LED_BUILTIN); // light the builtin LED if we power throttle.
  FastLED.setMaxPowerInMilliWatts(900);     // set the power limit, prevents drawing more amps than your supply can handle.
}

void loop() // put your main code here, to run repeatedly:
{
  bool bLED = 0; // LED state / starts OFF

  CFireEffect fire(NUM_LEDS, 20, 300, 3, 4, true, false);
  // uint8_t initialHue = 0;       // What color the strip starts at
  // const uint8_t deltaHue = 16;  // steps pattern through shades of 256 colors by rate of 16
  // const uint8_t hueDensity = 4; // High = high density, low = low density

  // BouncingBallEffect balls(NUM_LEDS, 3, 64); // inputs #leds, #balls, #dapening, Mirrored

  while (true)
  {
    FastLED.clear();
    fire.DrawFire();
    FastLED.show(g_Brightness);
    // Handle LEDs Different modes

    // fill_solid(g_LEDs, NUM_LEDS, CRGB::Green); // Solid mode

    // fill_rainbow(g_LEDs, NUM_LEDS, initialHue += hueDensity, deltaHue); // Rainbow mode

    // DrawMarquee();

    // DrawTwinkle();

    // DrawComet();

    // balls.Draw();

    // Handle OLED Drawing
    EVERY_N_MILLISECONDS(20)
    {
      /*
      fadeToBlackBy(g_LEDs, NUM_LEDS, 64);               // Make trail pixels fade out. fadeBy is a time interval.
      int cometSize = 15;                                // QTY of pixels in commet
      int iPos = beatsin16(12, 0, NUM_LEDS - cometSize); // speed of comet in bpm 16bits
      byte hue = beatsin8(30);                           // how often hue changes per min
      for (int i = iPos; i < iPos + cometSize; i++)
        // g_LEDs[i] = CHSV(hue, 255, 255);
        g_LEDs[i] = CRGB::Red;
      */

      // DrawMarqueeComparison();
    }

    EVERY_N_MILLISECONDS(250)
    {
      g_OLED.clearBuffer();
      g_OLED.setCursor(3, g_lineHeight); // placeholder for first line
      g_OLED.printf("Power : %u mW", calculate_unscaled_power_mW(g_LEDs, NUM_LEDS));
      g_OLED.setCursor(3, g_lineHeight * 2);
      g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
      g_OLED.setCursor(3, g_lineHeight * 3); // placeholder for framework
      g_OLED.printf("FPS: %u", FastLED.getFPS());
      g_OLED.sendBuffer();
    }

    FastLED.setBrightness(g_Brightness); // set the brightness scale
    FastLED.delay(33);                   // set the delay between loops
  }
}