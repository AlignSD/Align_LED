#include <Arduino.h>
#include <U8g2lib.h>

#define OLED_CLOCK 15 // pins for the oled display
#define OLED_DATA 4   //
#define OLED_RESET 16

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;

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

// DrawLinesAndGraphicsFram
//
// Draw a moire pattern with a box around it, a reticle and some text

void DrawLinesAndGraphicsFrame(double framesPerSecond)
{
  g_OLED.clearBuffer();
  g_OLED.home();

  g_OLED.drawFrame(0, 0, g_OLED.getWidth(), g_OLED.getHeight()); // Draw a border around the display

  // Draw some text into the frame
  g_OLED.setCursor(3, g_lineHeight * 2 + 2); // placeholder for first line
  g_OLED.print("Hello");
  g_OLED.setCursor(3, g_lineHeight * 3 + 2); // placeholder for second  line
  g_OLED.print("World");
  g_OLED.setCursor(3, g_lineHeight * 4 + 2); // placeholder for framework
  g_OLED.printf("FPS: %.1lf", framesPerSecond);

  // Draw a moire pattern like it's 1994
  for (int x = 0; x < g_OLED.getWidth(); x += 10)
    g_OLED.drawLine(x, 0, g_OLED.getWidth() + x, g_OLED.getHeight() / 1);

  // Draw a reticle on the right hand side

  const int reticleY = g_OLED.getHeight() / 2;           // Verticle center of display
  const int reticleR = g_OLED.getHeight() / 4 - 5;       // Slightly less than 1/4 screen height
  const int reticleX = g_OLED.getWidth() - reticleR - 4; // Right justified plus a margin

  for (int r = reticleR; r > 0; r -= 3) // Draw series of circles
    g_OLED.drawCircle(reticleX, reticleY, r);

  g_OLED.drawHLine(reticleX - reticleR, reticleY, 2 * reticleR); // H line through reticle center
  g_OLED.drawVLine(reticleX, reticleY - reticleR, 2 * reticleR); // V line through reticle center

  g_OLED.sendBuffer(); // Seeeeeennnnnnddddd It
}

void setup() // put your setup code here, to run once:
{
  pinMode(LED_BUILTIN, OUTPUT); // sets pin-mode to output for builtin LED

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);

  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // Descent is a negative number so we add it to total
}

void loop() // put your main code here, to run repeatedly:
{
  // bool bLED = 0; // LED state / starts OFF
  double fps = 0;

  for (;;)
  {
    // bLED = !bLED; // Blink the LED off and on
    // digitalWrite(LED_BUILTIN, bLED);

    double dStart = millis() / 1000.0; // Display a frame and calc how long it takes
    DrawLinesAndGraphicsFrame(fps);
    double dEnd = millis() / 1000.0;
    fps = FramesPerSecond(dEnd - dStart);
  }
}