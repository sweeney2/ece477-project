#ifndef __MAHOU_RENDER
#define __MAHOU_RENDER


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glib.h>

#include "mahou-kagami.h"


int
render(state_st* state, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture** assets, TTF_Font** fonts);


#endif  // __MAHOU_RENDER

