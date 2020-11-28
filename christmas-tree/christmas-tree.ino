// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

#include "application.h"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN A5
#define PIXEL_COUNT 480
#define MAX_LIT_PIXEL_COUNT 9
#define PIXEL_TYPE WS2812B
#define BRIGHTNESS 20

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

uint8_t numberOfZones = 10;
int zone1Start = 0;
int zone1End   = (PIXEL_COUNT / 10) - 1;
int zone3Start = (PIXEL_COUNT / 10);
int zone3End   = ((PIXEL_COUNT / 10) * 2) - 1;
int zone5Start = ((PIXEL_COUNT / 10) * 2);
int zone5End   = ((PIXEL_COUNT / 10) * 3) - 1;
int zone7Start = ((PIXEL_COUNT / 10) * 3);
int zone7End   = ((PIXEL_COUNT / 10) * 4) - 1;
int zone9Start = ((PIXEL_COUNT / 10) * 4);
int zone9End   = ((PIXEL_COUNT / 10) * 5) - 1;

int zone10Start = ((PIXEL_COUNT / 10) * 5);
int zone10End   = ((PIXEL_COUNT / 10) * 6) - 1;
int zone8Start  = ((PIXEL_COUNT / 10) * 6);
int zone8End    = ((PIXEL_COUNT / 10) * 7) - 1;
int zone6Start  = ((PIXEL_COUNT / 10) * 7);
int zone6End    = ((PIXEL_COUNT / 10) * 8) - 1;
int zone4Start  = ((PIXEL_COUNT / 10) * 8);
int zone4End    = ((PIXEL_COUNT / 10) * 9) - 1;
int zone2Start  = ((PIXEL_COUNT / 10) * 9);
int zone2End    = ((PIXEL_COUNT / 10) * 10) - 1;

uint8_t numModes = 11;
enum {
  mode_off = 0,
  mode_christmas_zone = 1,
  mode_zone = 2,
  mode_zone_up = 3,
  mode_christmas_zone_fill_up = 4,
  mode_rainbow = 5,
  mode_frozen = 6,
  mode_christmas_fill_up = 7,
  mode_rainbow_cycle = 8,
  mode_twinkly_color = 9,
  mode_xmas = 10,
  mode_comet = 11,
};

byte mode = mode_christmas_zone;
uint32_t offColor = strip.Color(0, 0, 0);
uint32_t redColor = strip.Color(255, 0, 0);
uint32_t greenColor = strip.Color(0, 255, 0);
uint32_t blueColor = strip.Color(0, 0, 255);
uint32_t whiteColor = strip.Color(255,255,255);
uint32_t dimmedWhiteColor = strip.Color(100, 100, 100);
uint32_t twinklyColor = redColor;
int brightness = BRIGHTNESS;
bool stop = FALSE;

uint32_t zoneColors[] = {
    offColor, offColor, offColor, offColor, offColor, offColor, offColor, offColor, offColor, offColor
};

uint32_t christmasZoneColors[] = {
    redColor, whiteColor, redColor, whiteColor, redColor, whiteColor, redColor, whiteColor, redColor, whiteColor
};
uint8_t numTwinklyColors = 4;
uint32_t twinklyColors[] = {
    redColor, greenColor, blueColor, dimmedWhiteColor
};

int8_t christmasFillUpLevels = 0;

uint32_t cycleWait = 200;

int changeModeMillisAfterFunctionCall = 10 * 60 * 1000;
int changeModeMillis = 30 * 1000;
unsigned long nextChangeModeMillis;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Particle.function("params", handleParams);
  nextChangeModeMillis = millis() + changeModeMillis;
}

unsigned long nextUpdateMillis = 0;
unsigned long animationStartTime = 0;

float getStepFloat(long t0, long t, float v) {
  return ((millis()-t0)%t)*v/t;
}

void loop() {
    stop = FALSE;
    
    unsigned long currentMillis = millis();
    if (mode != mode_off && currentMillis > nextChangeModeMillis) {
        nextChangeModeMillis += changeModeMillis;
        mode++;
        if (mode > numModes) {
            mode = 1;
        }
        setMode(mode);
    }
    
    switch (mode) {
        case mode_off: {
            for (int i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, offColor);
            }
            break;
        }
        case mode_zone: {
            zone();
            break;
        }
        case mode_zone_up: {
            zone();
            uint32_t newZoneColors[] = {
                zoneColors[9], zoneColors[0], zoneColors[1], zoneColors[2], zoneColors[3],
                zoneColors[4], zoneColors[5], zoneColors[6], zoneColors[7], zoneColors[8]
            };
            for (uint8_t i = 0; i < numberOfZones; i++) {
                zoneColors[i] = newZoneColors[i];
            }
            break;
        }
        case mode_christmas_zone: {
            for (uint8_t i = 0; i < numberOfZones; i++) {
                zoneColors[i] = christmasZoneColors[i];
            }
            mode = mode_zone;
            break;
        }
        case mode_christmas_zone_fill_up: {
            christmasZoneFillUp();
            break;
        }
        case mode_christmas_fill_up: {
            christmasFillUp();
            break;
        }
        case mode_frozen:
        case mode_twinkly_color: {
            frozen();
            break;
        }
        case mode_rainbow: {
            rainbow();
            break;
        }
        case mode_rainbow_cycle: {
            rainbowCycle();
            break;
        }
        case mode_xmas: {
          if (currentMillis > nextUpdateMillis) {
            byte r, g, b;
            for(int i=0; i<strip.getNumLeds(); i++) {
              r = 0, g = 0, b = 0;
              int rnd = random(99);
              if (rnd < 33) {
                r = 255;
              } else if (rnd < 66) {
                g = 255;
              } else {
                b = 255;
              }
              strip.setColor(i, r, g, b);
            }
            nextUpdateMillis = currentMillis + 500;
          }
          break;
        }
        case mode_comet: {
            int cometBrightness = 255;
            float l = strip.getNumLeds()/6;  // length of the tail
        	float t = getStepFloat(0, 3000, 2*strip.getNumLeds()-l);
        	uint8_t
              r = (uint8_t)(twinklyColor >> 16),
              g = (uint8_t)(twinklyColor >>  8),
              b = (uint8_t)twinklyColor;
        
        	for (int x = 0; x < strip.getNumLeds(); x++) {
        		strip.setColorDimmed(x, r, g, b, (int) constrain((((x-t)/l+1.2f))*(((x-t)<0)? 1:0)*cometBrightness, 0, cometBrightness));
        	}
        }
    }
	showPixels();
    delay(cycleWait);
}

void setMode(uint8_t newMode) {
    mode = newMode;
    switch (mode) {
        case mode_zone_up:
        case mode_christmas_zone:
        case mode_christmas_zone_fill_up: {
            cycleWait = 500;
            break;
        }
        case mode_christmas_fill_up: {
            cycleWait = 1;
            break;
        }
        case mode_frozen: {
            cycleWait = 100;
            break;
        }
        case mode_twinkly_color: {
            twinklyColor = twinklyColors[random(numTwinklyColors)];
            cycleWait = 100;
            break;
        }
        case mode_rainbow:
        case mode_rainbow_cycle:
        case mode_xmas: {
            cycleWait = 10;
            break;
        }
        case mode_comet: {
            twinklyColor = twinklyColors[random(numTwinklyColors)];
            cycleWait = 10;
            break;
        }
    }
}

void zone() {
	setZone(zone1Start, zone1End, zoneColors[0]);
	setZone(zone2Start, zone2End, zoneColors[1]);
	setZone(zone3Start, zone3End, zoneColors[2]);
	setZone(zone4Start, zone4End, zoneColors[3]);
	setZone(zone5Start, zone5End, zoneColors[4]);
	setZone(zone6Start, zone6End, zoneColors[5]);
	setZone(zone7Start, zone7End, zoneColors[6]);
	setZone(zone8Start, zone8End, zoneColors[7]);
	setZone(zone9Start, zone9End, zoneColors[8]);
	setZone(zone10Start, zone10End, zoneColors[9]);
}

void setZone(uint16_t zoneStart, uint16_t zoneEnd, uint32_t zoneColor) {
    for(uint16_t i=zoneStart; i<=zoneEnd; i++) {
        strip.setPixelColor(i, zoneColor);
	}
}

int8_t christmasFillDirection = 1;
int16_t christmasFillUpIndex = 0;

void christmasZoneFillUp() {
    for (uint8_t i = 0; i < numberOfZones; i++) {
        zoneColors[i] = offColor;
    }
    for (uint8_t i = 0; i < christmasFillUpLevels; i++) {
        zoneColors[i] = christmasZoneColors[i];
    }
    zone();
    if (christmasFillDirection == 1 && christmasFillUpLevels == numberOfZones) {
        christmasFillDirection = -1;
        strip.show();
        delay(3000);
    }
    if (christmasFillDirection == -1 && christmasFillUpLevels == 0) {
        christmasFillDirection = 1;
    }
    christmasFillUpLevels += christmasFillDirection;
}

void christmasFillUp() {
    uint16_t halfStrip = (PIXEL_COUNT / 2);
    for (int i = 0; i < halfStrip; i++) {
        if (i < christmasFillUpIndex) {
            strip.setPixelColor(i, redColor);
        } else {
            strip.setPixelColor(i, offColor);
        }
    }
    for (int i = (PIXEL_COUNT - 1); i >= (PIXEL_COUNT / 2); i--) {
        if (i > (PIXEL_COUNT - 1 - christmasFillUpIndex)) {
            strip.setPixelColor(i, whiteColor);
        } else {
            strip.setPixelColor(i, offColor);
        }
    }
    if (christmasFillDirection == 1 && christmasFillUpIndex == (PIXEL_COUNT / 2)) {
        delay(1000);
        christmasFillDirection = -1;
    }
    if (christmasFillDirection == -1 && christmasFillUpIndex == 0) {
        christmasFillDirection = 1;
    }
    christmasFillUpIndex += christmasFillDirection;
}

//Fade from teal to blue to purple then back to blue then teal, repeat
//Random snowflakes twinkle white for random amounts of time 
//The snowflakes will hang around no faster than MIN_DELAY and no slower than MAX_DELAY
int randomFlakes[(int)(PIXEL_COUNT*0.1)];

void frozen(void) {
  uint16_t i, j;
  uint16_t startColor = 140;    //Hue for teal
  uint16_t stopColor  = 210;    //Hue for purple
  const int MIN_DELAY = 400;	//in ms
  const int MAX_DELAY = 700;	//in ms
  const int MIN_FLAKES = strip.numPixels()*0.01;	//  1% of total number of pixels
  const int MAX_FLAKES = strip.numPixels()*0.1;	// 10% of total number of pixels
  int numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES);	//How many flakes should we have at a time
  unsigned long previousMillis = millis();
  unsigned long flakeLifeSpan = random(MIN_DELAY, MAX_DELAY); //How long will a snowflake last

  //forwards
  for(j=startColor; j<stopColor; j++) { 	//cycle through the colors
    for(i=0; i< strip.numPixels(); i++) {
        uint32_t pixelColor = twinklyColor;
        if (mode == mode_frozen) {
            pixelColor = Wheel(j);
        }
        strip.setPixelColor(i, pixelColor);	//first set all pixels to the same color
        if(millis() - previousMillis > flakeLifeSpan) {
			previousMillis = millis();						//reset time interval
			flakeLifeSpan = random(MIN_DELAY, MAX_DELAY);	//set new lifespan	
			numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES); //set new number of flakes
            findRandomSnowFlakesPositions(numSnowFlakes);	//get the list of flake positions
        }
    }
    if(stop == TRUE) {return;}
	setRandomSnowFlakes(numSnowFlakes);	//now set the snowflake positions
	showPixels();
    delay(cycleWait);
  }
  
  //backwards
  for(j=stopColor; j>startColor; j--) { 	//cycle through the colors
    for(i=0; i< strip.numPixels(); i++) {
        uint32_t pixelColor = twinklyColor;
        if (mode == mode_frozen) {
            pixelColor = Wheel(j);
        }
        strip.setPixelColor(i, pixelColor);	//first set all pixels to the same color
        if(millis() - previousMillis > flakeLifeSpan) {
			previousMillis = millis();						//reset time interval
			flakeLifeSpan = random(MIN_DELAY, MAX_DELAY);	//set new lifespan	
			numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES); //set new number of flakes
            findRandomSnowFlakesPositions(numSnowFlakes);	//get the list of flake positions
        }
    }
    if(stop == TRUE) {return;}
	setRandomSnowFlakes(numSnowFlakes);	//now set the snowflake positions
    showPixels();
    delay(cycleWait);
  }
}

void setRandomSnowFlakes(int numFlakes) {
    for(uint16_t i=0;i<numFlakes;i++) {
        strip.setPixelColor(randomFlakes[i], 0xFFFFFF); //Set snowflake
    }
}

void findRandomSnowFlakesPositions(int numFlakes) {
    for(uint16_t i=0;i<numFlakes;i++) {
        randomFlakes[i] = random(strip.numPixels());
        if(i > 0) {
            for(int j=0;j<i;j++) {
				//make sure we didn't already set that position
                if(randomFlakes[i] == randomFlakes[j]){
                    do {
                        randomFlakes[i] = random(strip.numPixels());
                    }while(randomFlakes[i] != randomFlakes[j]);
                }
            }
        }
    }
}

void rainbow(void) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    if(stop == TRUE) {return;}
    showPixels();
	delay(cycleWait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(void) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    if(stop == TRUE) {return;}
    showPixels();
	delay(cycleWait);
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void showPixels(void) {
    if (mode != mode_comet) {
	    strip.setBrightness(brightness);
    }
    strip.show();
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
    if (key=="brightness") {
      brightness = val;
    } else if (key=="cycleWait") {
      cycleWait = val;
    } else if (key=="changeModeMillis") {
      changeModeMillis = val;
    } else if (key=="changeModeMillisAfterFunctionCall") {
      changeModeMillisAfterFunctionCall = val;
    } else if (key=="mode") {
      setMode(val);
      nextChangeModeMillis = millis() + changeModeMillisAfterFunctionCall;
      stop = TRUE;
    } else if (key=="color" || key=="c") {
      if (value=="red") {
          twinklyColor = redColor;
      } else if (value=="green") {
          twinklyColor = greenColor;
      } else if (value=="white") {
          twinklyColor = dimmedWhiteColor;
      } else if (value=="rainbow") {
          setMode(mode_rainbow);
          nextChangeModeMillis = millis() + changeModeMillisAfterFunctionCall;
          stop = TRUE;
      }
    }
   p = i+1;
  }
  return 1;
}