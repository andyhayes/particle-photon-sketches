nclude "clickButton/clickButton.h"

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
String iconName = "message";
String text = "Welcome";
int textLength = text.length();
// default position of the text is outside and then scrolls left
int textX = bitmapWidth;
int fontWidth = 5, space = 1, iconWidth = 8, iconHeight = 8, displayHeight = 8;

// draw text
void drawText(String s, int x) {
  led->fillScreen(false);
  int y = 0;
  for(int i = 0; i < s.length(); i++) {
    led->drawChar(x + i*(fontWidth+space), y, s[i], true, false, 1);
  }
}

void drawTextWithIcon(String s, String iName, int x) {
  led->fillScreen(false);
  int y = 0;
  for(int i = 0; i < s.length(); i++) {
    led->drawChar(x + iconWidth + space + i*(fontWidth+space), y, s[i], true, false, 1);
  }
  led->fillRect(0, 0, iconWidth + space, displayHeight, false);
  uint8_t icon[8];
  iconFromName(iName, icon);
  led->drawBitmap(0, 0, icon, iconWidth, displayHeight, true);
}

const int buttonPin1 = D0;
ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);

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
  pinMode(buttonPin1, INPUT_PULLUP);

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register
  
  // new LEDMatrix(displays per row, displays per column, CLK, CS, DIN);
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

int countdownEventTime = 1482649200;
String countdownEventIcon = "christmastree";
String countdownEventDescription = "Christmas";

void loop() {
  button1.Update();
  if (button1.clicks == 1) {
      mode = mode_countdown;
  }
  switch (mode) {
    case mode_off: {
      led->fillScreen(false);
      led->flush();
      break;
    }
    case mode_message: {
      if (counter < 2) {
        drawTextWithIcon(text, iconName, textX--);
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
      String clockFormat = clockFormatBlinkOn;
      if ((millis() % 1000) > 500) {
          clockFormat = clockFormatBlinkOff;
      }
      drawText(Time.format(Time.now(), clockFormat), 1);
      led->flush();
      break;
    }
    case mode_countdown: {
        int now = Time.now();
        int secondsRemaining = countdownEventTime - now;
        if (secondsRemaining > 0) {
            int daysRemaining = (secondsRemaining / SECONDS_IN_A_DAY) + 1;
            messageCommand(String::format("%s|%d more sleeps until %s!", countdownEventIcon.c_str(), daysRemaining, countdownEventDescription.c_str()));
        } else {
            messageCommand(String::format("%s|It's %s!", countdownEventIcon.c_str(), countdownEventDescription.c_str()));
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
    int i = command.indexOf('|',0);
    if (i > 0) {
      iconName = command.substring(0, i);
      text = command.substring(i+1, command.length());
    } else {
      iconName = "message";
      text = command;
    }
    resetMessage();
    mode = mode_message;
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
