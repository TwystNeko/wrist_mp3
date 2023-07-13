#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <stdint.h>
//#include <GC9A01A_t3n.h>
#include "defines.h"
//#include <lv_conf.h>
#include <lvgl.h>
#include <SD.h>
#include "sdios.h"
#include "ui.h"
#include "ui_helpers.h"
#include <Audio.h>
#include <play_sd_mp3.h>


#define T_SDA 25
#define T_SCL 24
#define T_RST 37
#define T_INT 40

#define TFT_DC  28
#define TFT_CS 38
#define TFT_RST 41
#define TFT_SCK 27
#define TFT_MISO 39
#define TFT_MOSI 26
#define TFT_BL 29
#define T_ADDR 0x15
#define T_MODE FALLING
#define SD_FAT_TYPE 3

#define MAX_SONGS 64



void my_input_read(lv_indev_drv_t *, lv_indev_data_t*);
void read_touch();
uint8_t i2c_read(uint8_t , uint8_t , uint8_t , uint8_t );
uint8_t i2c_write(uint8_t, uint8_t, uint8_t, uint8_t);

void touch_init();

void build_song_list(File dir);
void playFile();
void play_selected(lv_event_t);
void update_player_screen();
void controls();
void play_prevSong(lv_event_t);
void play_nextSong(lv_event_t);
void play_pause(lv_event_t);
void play_stop(lv_event_t);


song_data songs[MAX_SONGS];
int songidx = 0;
int max_songidx = 0;
touch_data tdata;
bool trackchange;
uint32_t songEnd;
uint32_t nextFrame;
bool paused;
bool stopped;
bool freshSong;

static uint32_t screenWidth = 240;
static uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;


AudioPlaySdMp3           playMp31;       //xy=154,78
AudioOutputI2S           i2s1;           //xy=334,89

AudioControlSGTL5000     sgtl5000_1;     //xy=240,153
AudioMixer4 mixleft;
AudioMixer4 mixright;

AudioConnection          patch1(playMp31, 0, mixleft, 0);
AudioConnection          patch2(playMp31, 1, mixright, 1);

AudioConnection          patch3(mixleft, 0, i2s1, 0);
AudioConnection          patch4(mixright, 0, i2s1, 1);    

Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC,TFT_CS,TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 3, true);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}



void setup() {
  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 1023);


  gfx->begin();
  gfx->fillScreen(BLACK);
  Serial.begin(9600);
  touch_init();
  lv_init();

  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 40);
  if(!disp_draw_buf) { 
    Serial.println("LVGL disp_draw_buf allocate failed");
  }
  else 
  { 
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 40);
        lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    // set up input
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_input_read;
    lv_indev_drv_register(&indev_drv);

    // Grab file list into an array. Store filename and quick name. 
    if(!SD.begin(BUILTIN_SDCARD)) { 
      Serial.println("SD Init Failed.");
      return;
    }
    Serial.println("SD Init Successful");
    File root = SD.open("/");
    ui_init();
    build_song_list(root);

    // Audio Setup
      sgtl5000_1.enable();
      sgtl5000_1.volume(.5);
      mixleft.gain(0,0.9);
      mixright.gain(0,0.9);
      AudioMemory(16);

    Serial.println("System Started!");
  }

  
}

void build_song_list(File dir) { 
  String makeSongList;
  
  Serial.println("building song list!");
  String fname;
  while(true) { 
    File entry = dir.openNextFile();
    if(!entry) { 
      break;
    }
    if(entry.isDirectory()) { 
      Serial.println("here's a dir.");
      // do nothing, all songs are in root
    } else { 
      Serial.println("here's a file.");
      fname = entry.name();
      Serial.println(fname);
      String suffix = ".mp3";
      if(fname.endsWith(suffix)) { 
        // it's an mp3, let's break down the name.
        // entry.name() just goes into the filename. 
        makeSongList.append("\n");
        songs[songidx].fileName = fname;
        songs[songidx].Artist = fname.substring(0,fname.indexOf('-',0));
        songs[songidx].songName = fname.substring(fname.indexOf('-',0)+1, fname.lastIndexOf('-'));
        makeSongList.append(songs[songidx].songName);
      

        songs[songidx].length = fname.substring(fname.lastIndexOf('-')+1, fname.lastIndexOf('.')).toInt();
        songidx++;
      }


    }



  }
  makeSongList.append("\0");
  
  max_songidx = songidx;

  lv_dropdown_set_options(ui_songList, makeSongList.substring(1).c_str());

  
}



void play_selected(lv_event_t * e)
{
	// sets songidx, goes to playing file. 
	songidx = lv_dropdown_get_selected(ui_songList);

  playFile();


}

void playFile() { 

  playMp31.play(songs[songidx].fileName.c_str());
  freshSong = true;
  stopped = false;
}


void play_prevSong(lv_event_t * e)
{
	// Your code here
  songidx--;
  if(songidx < 0) { songidx = max_songidx; }
  playFile();

}

void play_pause(lv_event_t * e)
{
	// Your code here
  paused = playMp31.pause(!paused);
  if(paused) { 
    lv_imgbtn_set_state(ui_playPause, LV_IMGBTN_STATE_CHECKED_RELEASED);
  }  else { 
    lv_imgbtn_set_state(ui_playPause, LV_IMGBTN_STATE_RELEASED);
  }
}

void play_stop(lv_event_t * e)
{
	// Your code here
  playMp31.stop();  
  stopped = true;
  lv_dropdown_set_selected(ui_songList, songidx);
}

void play_next_song(lv_event_t * e)
{
	// Your code here
  songidx++;
  songidx = songidx % max_songidx;
  playFile();

}


void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) { 
  //Serial.println("read_cb");

    read_touch();
      // if points = 1, it's still pressed. points = 0 is released. Ignore gesture for now?
  if(tdata.points) { 
    data->point.x = tdata.x;
    data->point.y = tdata.y;
    data->state = LV_INDEV_STATE_PR;
  } else { 
    data->state = LV_INDEV_STATE_REL;
  }
  
}
void controls() { 
  // check volume and autoplay
    float vol = analogRead(15);
    vol = vol / 1024;
    sgtl5000_1.volume(vol);
    if(lv_obj_has_state(ui_autoPlay, LV_STATE_CHECKED)) { 
      trackchange = true;
    } else { 
      trackchange = false;
    }
}

void loop() {
/*
  if(lv_obj_has_state(ui_autoPlay, LV_STATE_CHECKED)) { 
    trackchange = true;
  } else { 
    trackchange = false;
  }
  lv_timer_handler();
  delay(5);
  // Volume control! Wee!
   float vol = analogRead(15);
    vol = vol / 1024;
    sgtl5000_1.volume(vol);
  
    if(playMp31.isPlaying()) { 
      // update counting 
      update_player_screen();
    } else { 
        if(trackchange && !stopped) { 
          songidx++;
          
        }
      
    }
    */
   while(playMp31.isPlaying()) { 
    // we doing song!
    update_player_screen();
    lv_timer_handler();
    controls();
   } 
   // not playing song. Did it get force stopped? 
   if(stopped) { 
      // okay, was stopped via long-press. Back at PickerScreen (auto handled by LVGL)
      lv_dropdown_set_selected(ui_songList, songidx);
   } else { 
      // are we jumping to the next song?
      if(trackchange) { 
        songidx++;
        songidx = songidx % max_songidx; // keep it in the playlist bounds.
        playFile();
      } else { // go back to PickerScreen
        lv_scr_load_anim(ui_PickerScreen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 200, 0, true);
      }
   }

    update_player_screen();
    lv_timer_handler();
    controls();
}

void update_player_screen() { 
  if(freshSong) { 
    // only do this once!
    lv_label_set_text(ui_SongTitle, songs[songidx].songName.c_str());
    lv_label_set_text(ui_Artist, songs[songidx].Artist.c_str() );
    lv_imgbtn_set_state(ui_playPause, LV_IMGBTN_STATE_CHECKED_RELEASED);
    freshSong = false;
  }
  char timeLabel[16];
  int s = songs[songidx].length;
  int sm = floor(s/60);
  int ss = s % 60;

  u_int c = playMp31.positionMillis() / 1000; // how many seconds have been played. Alas, this does not always match up with songs[songidx].length;

  int cm = floor(c/60);
  int cs = c % 60;
  sprintf(timeLabel, "%02d:%02d / %02d:%02d", cm, cs, sm, ss);
  lv_label_set_text(ui_Time, timeLabel);

}




uint8_t i2c_read(uint16_t addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t length) { 
  Wire2.beginTransmission(addr);
  Wire2.write(reg_addr);
  if(Wire2.endTransmission(true)) return -1;

  Wire2.requestFrom((uint8_t)addr,(uint8_t)length,(bool)true);
  for(uint8_t i = 0; i < length; i++) { 
    *reg_data++ = Wire2.read();
  }
  return 0;
}

uint8_t i2c_write(uint8_t addr, uint8_t reg_addr, const uint8_t *reg_data, uint8_t length) { 
  Wire2.beginTransmission(addr);
  Wire2.write(reg_addr);
  for(uint8_t i = 0; i < length; i++) { 
    Wire2.write(*reg_data++);
  }
  if(Wire2.endTransmission(true)) return -1;
  return 0;
}


void read_touch() { 
  byte raw_touch[8];
  i2c_read(T_ADDR, 0x01, raw_touch, 6);
  tdata.gestureID = raw_touch[0];
  tdata.points = raw_touch[1];
  tdata.event = raw_touch[2]>>6;
 // tdata.x = raw_touch[3];
 // tdata.y = raw_touch[5];
  tdata.x = 240 - raw_touch[5];
  tdata.y = raw_touch[3];


}

void touch_init() { 
  Wire2.setSCL(T_SCL);
  Wire2.setSDA(T_SDA);
  Wire2.begin();
  pinMode(T_INT, INPUT);
  pinMode(T_RST, OUTPUT);

  digitalWrite(T_RST, HIGH);
  delay(50);
  digitalWrite(T_RST, LOW);
  delay(5);
  digitalWrite(T_RST, HIGH);
  delay(50);

  i2c_read(T_ADDR,0x15, &tdata.version, 1);
  delay(5);
  i2c_read(T_ADDR, 0xA7, tdata.versionInfo, 3);
 // attachInterrupt(T_INT, handleISR, T_MODE);

}
