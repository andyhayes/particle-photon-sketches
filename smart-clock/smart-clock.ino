#include "ledmatrix-max7219-max7221/ledmatrix-max7219-max7221.h"
#include "icons.h"

#define SECONDS_IN_A_DAY (60 * 60 * 24)
#define SECONDS_IN_AN_HOUR (60 * 60)
#define SECONDS_IN_A_MINUTE 60

LEDMatrix *led;

enum {
  mode_off = 0,
  mode_message = 1,
  mode_clock = 2,
  mode_countdown = 3,
};

uint8_t mode = mode_clock;
uint16_t cycleWait = 50;

int numberOfDisplays = 4;
int bitmapWidth = numberOfDisplays * 8;
// cycle counter (incremented after each heart drawing)
int counter = 0;
String text = "Welcome";
int textLength = text.length();
// default position of the text is outside and then scrolls left
int textX = bitmapWidth;
int fontWidth = 5, space = 1, iconWidth = 8, displayHeight = 8;

// draw text
void drawText(String s, int x) {
  led->fillScreen(false);
  int y = 0;
  for(int i = 0; i < s.length(); i++) {
    led->drawChar(x + i*(fontWidth+space), y, s[i], true, false, 1);
  }
}

void drawTextWithIcon(String s, int x) {
  led->fillScreen(false);
  int y = 0;
  for(int i = 0; i < s.length(); i++) {
    led->drawChar(x + iconWidth + space + i*(fontWidth+space), y, s[i], true, false, 1);
  }
  led->fillRect(0, 0, iconWidth + space, displayHeight, false);
  led->drawBitmap(0, 0, CAR_ICON, iconWidth, displayHeight, true);
}

// draw symbol of heart
void drawHeart() {
  int x = 1, y = 2;
  // methods from Adafruit_GFX
  led->fillCircle(x+1, y+1, 1, true);
  led->fillCircle(x+5, y+1, 1, true);
  led->fillTriangle(x+0, y+1, x+6, y+1, x+3, y+4, true);
  // direct access to the main bitmap (instance of LEDMatrix::Bitmap)
  led->bitmap->setPixel(x+3, y+1, false);
}

void setup() {
  // setup pins and library
  // 1 display per row, 1 display per column
  // optional pin settings - default: CLK = A0, CS = A1, D_OUT = A2
  // (pin settings is independent on HW SPI)
  led = new LEDMatrix(numberOfDisplays, 1, A3, A4, A5);
  // > add every matrix in the order in which they have been connected <
  // the first matrix in a row, the first matrix in a column
  // vertical orientation (-90°) and no mirroring - last three args optional
  for (int i = 0; i < numberOfDisplays; i++) {
    led->addMatrix(i, 0, 180, false, false);
  }
  led->setIntensity(1);
  Particle.function("message", messageCommand);
  Particle.function("countdown", countdownCommand);
  Particle.function("params", handleParams);
}

String clockFormatBlinkOn = "%H:%M";
String clockFormatBlinkOff = "%H %M";
String clockFormat = clockFormatBlinkOn;

int countdownEventTime = 1482649200;
String countdownEventDescription = "Christmas";

void loop() {
  switch (mode) {
    case mode_off: {
      led->fillScreen(false);
      led->flush();
      break;
    }
    case mode_message: {
      if (counter < 2) {
        drawTextWithIcon(text, textX--);
        // text animation is ending when the whole text is outside the bitmap
        if (textX < textLength*(fontWidth+space)*(-1)) {
          // set default text position
          textX = bitmapWidth;
          counter++;
        }
        led->flush();
      } else {
          resetMessage();
          mode = mode_clock;
      }
      break;
    }
    case mode_clock: {
      drawText(Time.format(Time.now(), clockFormat), 1);
      led->flush();
      if (clockFormat == clockFormatBlinkOn) {
          clockFormat = clockFormatBlinkOff;
      } else {
          clockFormat = clockFormatBlinkOn;
      }
      delay(500 - cycleWait);
      break;
    }
    case mode_countdown: {
        int now = Time.now();
        int secondsRemaining = countdownEventTime - now;
        if (secondsRemaining > 0) {
            int daysRemaining = secondsRemaining / SECONDS_IN_A_DAY;
            secondsRemaining = secondsRemaining - (daysRemaining * SECONDS_IN_A_DAY);
            int hoursRemaining = secondsRemaining / SECONDS_IN_AN_HOUR;
            secondsRemaining = secondsRemaining - (hoursRemaining * SECONDS_IN_AN_HOUR);
            int minutesRemaining = secondsRemaining / SECONDS_IN_A_MINUTE;
            secondsRemaining = secondsRemaining - (minutesRemaining * SECONDS_IN_A_MINUTE);
            
            messageCommand(String::format("It is %d days, %d hours, %d minutes and %d seconds until %s!",
                daysRemaining, hoursRemaining, minutesRemaining, secondsRemaining, countdownEventDescription.c_str()));
        } else {
            messageCommand(String::format("It's %s!", countdownEventDescription.c_str()));
        }
        break;
    }
  }
  delay(cycleWait);
}

void resetMessage() {
    textLength = text.length();
    textX = bitmapWidth;
    counter = 0;
}

int messageCommand(String command) {
    text = command;
    resetMessage();
    mode = mode_message;
    led->fillScreen(false);
    led->flush();
    return 0;
}

int countdownCommand(String command) {
    mode = mode_countdown;
    return 0;
}

int handleParams(String command) {
  // accepts params in format param1=value1,param2=value2
  int p = 0;
  while (p<(int)command.length()) {
    int i = command.indexOf(',',p);
    if (i<0) i = command.length();
    int j = command.indexOf('=',p);
    if (j<0) break;
    String key = command.substring(p,j);
    String value = command.substring(j+1,i);
    int val = value.toInt();
    if (key=="cycleWait")
      cycleWait = val;
    else if (key=="mode")
      mode = val;
    p = i+1;
  }
  return 1;
}
