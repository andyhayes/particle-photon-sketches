static const uint8_t CAR_ICON[] = {
  0B00111100,
  0B01000010,
  0B01010010,
  0B01111110,
  0B10000001,
  0B10111101,
  0B11111111,
  0B01000010
};

static const uint8_t CHRISTMAS_TREE_ICON[] = {
  0B00011000,
  0B00011000,
  0B00111100,
  0B01111110,
  0B00111100,
  0B01111110,
  0B11111111,
  0B00011000
};

static const uint8_t MESSAGE_ICON[] = {
  0B00000000,
  0B11111111,
  0B11000011,
  0B10100101,
  0B10011001,
  0B10000001,
  0B11111111,
  0B00000000
};

static const uint8_t RAIN_ICON[] = {
  0B00011100,
  0B01111110,
  0B11111111,
  0B11111111,
  0B01111110,
  0B00001000,
  0B01010010,
  0B10000100
};

static const uint8_t SUN_ICON[] = {
  0B10001001,
  0B01000010,
  0B00011000,
  0B10111100,
  0B00111101,
  0B00011000,
  0B01000010,
  0B10010001
};

static const uint8_t CLOUD_ICON[] = {
  0B00000000,
  0B00011100,
  0B01100010,
  0B10000001,
  0B10000001,
  0B01111110,
  0B00000000,
  0B00000000
};

static const uint8_t TIMER_ICON[] = {
  0B01111110,
  0B01111110,
  0B00111100,
  0B00011000,
  0B00011000,
  0B00100100,
  0B01000010,
  0B01111110
};

static const uint8_t CLOCK_ICON[] = {
  0B00111100,
  0B01010010,
  0B10010001,
  0B10010001,
  0B10011101,
  0B10000001,
  0B01000010,
  0B00111100
};

void iconFromName(String name, uint8_t* icon) {
    if (name == "car") {
        memcpy(icon, CAR_ICON, 8);
    } else if (name == "christmastree") {
        memcpy(icon, CHRISTMAS_TREE_ICON, 8);
    } else if (name == "rain") {
        memcpy(icon, RAIN_ICON, 8);
    } else if (name == "sun") {
        memcpy(icon, SUN_ICON, 8);
    } else if (name == "cloud") {
        memcpy(icon, CLOUD_ICON, 8);
    } else if (name == "timer") {
        memcpy(icon, TIMER_ICON, 8);
    } else if (name == "clock") {
        memcpy(icon, CLOCK_ICON, 8);
    } else {
        memcpy(icon, MESSAGE_ICON, 8);
    }
}

