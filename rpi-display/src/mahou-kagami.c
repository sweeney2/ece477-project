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
init (SDL_Window** window, SDL_Renderer** renderer, TTF_Font*** fonts)
{
  // TODO: These should be read in from a config file.
  //       They live here for now.
  fontInfo_st* font_info = (fontInfo_st*) malloc(FONT_COUNT * sizeof(fontInfo_st));

  fontInfo_st font_debug = {
    .path = "assets/Roboto-Regular.ttf",
    .size = 20
    };
  fontInfo_st font_clock = {
    .path = "assets/Roboto-Regular.ttf",
    .size = 120
    };
  fontInfo_st font_quote = {
    .path = "assets/Roboto-Regular.ttf",
    .size = 40
    };
  font_info[DEBUG] = font_debug;
  font_info[CLOCK] = font_clock;
  font_info[QUOTE] = font_quote;

  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(
        stderr,
        "SDL_Init failed: %s\n",
        SDL_GetError()
        );
    free(font_info);
    return -1;
  }

  // initialize SDL_TTF
  if (TTF_Init()) {
    fprintf(
        stderr,
        "TTF_Init failed: %s\n",
        TTF_GetError()
        );
    SDL_Quit();
    free(font_info);
    return -1;
  }

  // create main window
  *window = SDL_CreateWindow(
      "Magic Mirror",
      0,
      0,
      0,
      0,
      SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
      );
  if (!*window) {
    fprintf(
        stderr,
        "SDL_CreateWindow failed: %s\n",
        SDL_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    free(font_info);
    return -1;
  }

  // create main renderer
  *renderer = SDL_CreateRenderer(
      *window,
      -1,
      0
      );
  if (!*renderer) {
    fprintf(
        stderr,
        "SDL_CreateRenderer failed: %s\n",
        SDL_GetError()
        );
    SDL_DestroyWindow(*window);
    TTF_Quit();
    SDL_Quit();
    free(font_info);
    return -1;
  }

  // check font paths
  for (int i = 0; i < FONT_COUNT; i++) {
    if (access(font_info[i].path, F_OK) == -1) {
      fprintf(
          stderr,
          "[font %d/%d] Font not found: %s\n",
          i + 1,
          FONT_COUNT,
          font_info[i].path
          );
      SDL_DestroyRenderer(*renderer);
      SDL_DestroyWindow(*window);
      TTF_Quit();
      SDL_Quit();
      free(font_info);
      return -1;
    }
  }

  // allocate space for fonts
  *fonts = (TTF_Font**) malloc(FONT_COUNT * sizeof(**fonts));

  // create font objects
  for (int i = 0; i < FONT_COUNT; i++) {
    *(*fonts + i) = TTF_OpenFont(
        font_info[i].path,
        font_info[i].size
        );
    if (!*(*fonts + i)) {
      fprintf(
          stderr,
          "[font %d/%d] TTF_OpenFont failed: %s\n",
          i + 1,
          FONT_COUNT,
          TTF_GetError()
          );
      for (int j = 0; j < i; j++) {
        TTF_CloseFont(*(*fonts + j));
      }
      free(*fonts);
      SDL_DestroyRenderer(*renderer);
      SDL_DestroyWindow(*window);
      TTF_Quit();
      SDL_Quit();
      free(font_info);
      return -1;
    }
  }

  free(font_info);
  return 0;
}

int
main(void)
{
  // variable declarations
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  TTF_Font** fonts = NULL;

  // call initialization routine
  if (init(&window, &renderer, &fonts))
    return EXIT_FAILURE;

  // execute main loop
  while (update()) {
    render();
  }

  // cleanup
  for (int i = 0; i < FONT_COUNT; i++) {
    TTF_CloseFont(*(fonts + i));
  }
  free(fonts);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}

