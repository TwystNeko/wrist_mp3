#include <stdint.h>
#include <Arduino.h>


enum GESTURE { 
  NONE = 0x00,
  SWIPE_UP = 0x01,
  SWIPE_DOWN = 0x02,
  SWIPE_LEFT = 0x03,
  SWIPE_RIGHT = 0x04,
  SINGLE_CLICK = 0x05,
  DOUBLE_CLICK = 0x0B,
  LONG_PRESS = 0x0C
};

struct touch_data { 
  byte gestureID;
  byte points;
  byte event;
  int x;
  int y;
  uint8_t version;
  uint8_t versionInfo[3];
};

struct song_data { 
  String songName;
  String fileName;
  String Artist;
  int length;
};