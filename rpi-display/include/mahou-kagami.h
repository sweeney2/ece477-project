#ifndef __MAHOU_KAGAMI
#define __MAHOU_KAGAMI


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#include "mahou-update.h"
#include "mahou-render.h"


enum font {
  DEBUG,
  CLOCK,
  QUOTE,
  FONT_COUNT
};

typedef struct fontInfo_st {
  char path[80];
  int size;
} fontInfo_st;


int
init (SDL_Window** window, SDL_Renderer** renderer, TTF_Font*** fonts);

int
main(void);


#endif  // __MAHOU_KAGAMI

