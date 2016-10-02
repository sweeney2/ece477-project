#ifndef __MAHOU_KAGAMI
#define __MAHOU_KAGAMI


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#include "mahou-update.h"
#include "mahou-render.h"


int
init (SDL_Window** window, SDL_Renderer** renderer, TTF_Font*** fonts);

int
main(void);


#endif  // __MAHOU_KAGAMI



#ifndef __MAHOU_KAGAMI_ENUMS_AND_STRUCTS
#define __MAHOU_KAGAMI_ENUMS_AND_STRUCTS


enum font {
  DEBUG,
  CLOCK,
  QUOTE,
  FONT_COUNT
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
  WATER_YEAR
};


typedef struct fontInfo_st {
  char path[80];
  int size;
} fontInfo_st;

typedef struct state_st {
  enum screen current;
  long timer;
} state_st;


#endif  // __MAHOU_KAGAMI_ENUMS_AND_STRUCTS

