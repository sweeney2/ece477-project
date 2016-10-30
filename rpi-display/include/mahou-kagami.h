#ifndef __MAHOU_KAGAMI
#define __MAHOU_KAGAMI


#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#include "mahou-update.h"
#include "mahou-render.h"

#define CONFIG_FILE "magic.conf"

int
init (config_t* config, SDL_Window** window, SDL_Renderer** renderer, SDL_Texture*** assets, TTF_Font*** fonts);

int
main(int argc, char* argv[]);


#endif  // __MAHOU_KAGAMI



#ifndef __MAHOU_KAGAMI_ENUMS_AND_STRUCTS
#define __MAHOU_KAGAMI_ENUMS_AND_STRUCTS


enum font {
  DEBUG,
  CLOCK,
  QUOTE,
  FONT_COUNT
};

enum assets {
  ASSET_USB,
  ASSET_COUNT
};

enum screen {
  HOME,
  PHOTO,
  TIMER_READY,
  TIMER_ACTIVE,
  TIMER_PAUSE,
  TIMER_DONE,
  WATER_DAY,
  WATER_WEEK,
  WATER_MONTH,
  WATER_YEAR,
  SCREEN_COUNT,
  NONE
};

enum usb_status {
  USB_IDLE,
  USB_ADD,
  USB_DONE
};


typedef struct fontInfo_st {
  const char* path;
  int size;
} fontInfo_st;

typedef struct assetInfo_st {
  const char* path;
} assetInfo_st;

typedef struct state_st {
  enum screen current;
  long timer;
  enum usb_status usb;
} state_st;

typedef struct adjScreens_st {
  enum screen up;
  enum screen down;
  enum screen left;
  enum screen right;
} adjScreens_st;

typedef struct texture_st {
  SDL_Texture* texture;
  SDL_Rect rect;
} texture_st;


#endif  // __MAHOU_KAGAMI_ENUMS_AND_STRUCTS

