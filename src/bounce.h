//+-------------------------------------------------------------------------------------------------------
//
// Arduino LED Controller
//
// File:  marquee.h
//
// Description:
//
//  Draws pixels that bounce at the end of the strip ends.
//
//--------------------------------------------------------------------------------------------------------
#include <sys/time.h> // allows us to calculate time difference

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

using namespace std;
#include <vector> // c++ variable sized array

extern CRGB g_LEDs[];

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0])) // count elements in a static array

static const CRGB ballColors[] =
    {
        CRGB::Green,
        CRGB::Red,
        CRGB::Blue,
        CRGB::Orange,
        CRGB::Indigo};

class BouncingBallEffect
{
private:
  double InitialBallSpeed(double height) const
  {
    return sqrt(-2 * Gravity * height);
  };

  size_t _cLength;
  size_t _cBalls;
  byte _fadeRate;
  bool _bMirrored;

  const double Gravity = -9.81;
  const double StartHeight = 1;
  const double ImpactVelocity = InitialBallSpeed(StartHeight);
  const double SpeedKnob = 4.0;

  vector<double> ClockTimeAtLastBounce, Height, BallSpeed, Dampening;
  vector<CRGB> Colors;

  double Time() const
  {
    timeval tv = {0};
    gettimeofday(&tv, nullptr);
    return (double)(tv.tv_usec / 1000000.0 + (double)tv.tv_sec);
  }

public:
  // Bouncing ball effect
  //
  // caller specs strip length, number of balls, persistence level (255 is the least), and whether the
  // balls should be drawn mirrord from each side
  BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = false)
      : _cLength(cLength - 1),
        _cBalls(ballCount),
        _fadeRate(fade),
        _bMirrored(bMirrored),
        ClockTimeAtLastBounce(ballCount),
        Height(ballCount),
        BallSpeed(ballCount),
        Dampening(ballCount),
        Colors(ballCount)
  {
    for (size_t i = 0; i < ballCount; i++)
    {
      Height[i] = StartHeight;                    // current ball height
      ClockTimeAtLastBounce[i] = Time();          // When ball last hit ground state
      Dampening[i] = 0.90 - i / pow(_cBalls, 2);  // bounciness of this ball
      BallSpeed[i] = InitialBallSpeed(Height[i]); // dont dampen initial launch
      Colors[i] = ballColors[i % ARRAYSIZE(ballColors)];
    }
  }

  // Draw
  //
  // Draw each of the balls. When any ball settles with too little energy, it is kicked to restart

  virtual void Draw()
  {
    if (_fadeRate != 0)
    {
      for (size_t i = 0; i < _cLength; i++)
        g_LEDs[i].fadeToBlackBy(_fadeRate);
    }

    else
      FastLED.clear();

    // Draw each of the balls

    for (size_t i = 0; i < _cBalls; i++)
    {
      double TimeSinceLastbounce = (Time() - ClockTimeAtLastBounce[i]) / SpeedKnob;

      // ude standard constant acceleration function - https://en.wikipedia.org/wiki/acceleration
      Height[i] = 0.5 * Gravity * pow(TimeSinceLastbounce, 2.0) + BallSpeed[i] * TimeSinceLastbounce;

      // Ball hits ground - bounce

      if (Height[i] < 0)
      {
        Height[i] = 0;
        BallSpeed[i] = Dampening[i] * BallSpeed[i];
        ClockTimeAtLastBounce[i] = Time();

        if (BallSpeed[i] < 0.01)
          BallSpeed[i] = InitialBallSpeed(StartHeight) * Dampening[i];
      }

      size_t position = (size_t)(Height[i] * (_cLength - 1) / StartHeight);

      g_LEDs[position] += Colors[i];
      g_LEDs[position += 1] = Colors[i];

      if (_bMirrored)
      {
        g_LEDs[_cLength - 1 - position] += Colors[i];
        g_LEDs[_cLength - position] += Colors[i];
      }
      delay(20);
    }
  }
};
