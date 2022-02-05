#include <Arduino.h>
#include <U8g2lib.h>

#define OLED_CLOCK 15 // pins for the oled display
#define OLED_DATA 4   //
#define OLED_RESET 16

U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R2, OLED_CLOCK, OLED_DATA, OLED_RESET);
int g_lineHeight = 0;
void setup() // put your setup code here, to run once:
{
  pinMode(LED_BUILTIN, OUTPUT);

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);

  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // Descent is a negative number so we add it to total

  g_OLED.drawFrame(0, 0, g_OLED.getWidth(), g_OLED.getHeight()); // Draw a border around the display

  // Draw a moire pattern like it's 1994
  for (int x = 0; x < g_OLED.getWidth(); x += 4)
    g_OLED.drawLine(x, 0, g_OLED.getWidth() - x, g_OLED.getHeight());

  // Draw a reticle on the right hand side

  const int reticleY = g_OLED.getHeight() / 2;           // Verticle center of display
  const int reticleR = g_OLED.getHeight() / 4 - 2;       // Slightly less than 1/4 screen height
  const int reticleX = g_OLED.getWidth() - reticleR - 8; // Right justified plus a margin

  for (int r = reticleR; r > 0; r -= 3) // Draw series of circles
    g_OLED.drawCircle(reticleX, reticleY, r);

  g_OLED.drawHLine(reticleX - reticleR - 5, reticleY, 2 * reticleR + 10); // H line through reticle center
  g_OLED.drawVLine(reticleX, reticleY - reticleR - 5, 2 * reticleR + 10); // V line through reticle center

  g_OLED.sendBuffer(); // Seeeeeennnnnnddddd It
}

void loop() // put your main code here, to run repeatedly:
{
  digitalWrite(LED_BUILTIN, 0);
  delay(500);
  digitalWrite(LED_BUILTIN, 1);
  delay(500);
  digitalWrite(LED_BUILTIN, 0);
  delay(100);
  digitalWrite(LED_BUILTIN, 1);
  delay(100);
}