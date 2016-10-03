#include "mahou-kagami.h"

int
init (SDL_Window** window, SDL_Renderer** renderer, TTF_Font*** fonts)
{
  // TODO: These should be read in from a config file.
  //       They live here for now.

  // font settings
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
  // TODO: These should be read in from a config file.
  //       They live here for now.

  // screen settings
  adjScreens_st* adj_screens = (adjScreens_st*) malloc(SCREEN_COUNT * sizeof(adjScreens_st));

  adjScreens_st screen_home = {
    .up = PHOTO,
    .down = NONE,
    .left = TIMER_READY,
    .right = WATER_DAY
    };
  adjScreens_st screen_photo = {
    .up = NONE,
    .down = HOME,
    .left = NONE,
    .right = NONE
    };
  adjScreens_st screen_timer_ready = {
    .up = TIMER_ACTIVE,
    .down = NONE,
    .left = NONE,
    .right = HOME
    };
  adjScreens_st screen_timer_active = {
    .up = NONE,
    .down = TIMER_PAUSE,
    .left = NONE,
    .right = NONE
    };
  adjScreens_st screen_timer_pause = {
    .up = TIMER_ACTIVE,
    .down = NONE,
    .left = TIMER_READY,
    .right = NONE
    };
  adjScreens_st screen_timer_done = {
    .up = NONE,
    .down = NONE,
    .left = TIMER_READY,
    .right = HOME
    };
  adjScreens_st screen_water_day = {
    .up = WATER_WEEK,
    .down = NONE,
    .left = HOME,
    .right = NONE
    };
  adjScreens_st screen_water_week = {
    .up = WATER_MONTH,
    .down = WATER_DAY,
    .left = HOME,
    .right = NONE
    };
  adjScreens_st screen_water_month = {
    .up = WATER_YEAR,
    .down = WATER_WEEK,
    .left = HOME,
    .right = NONE
    };
  adjScreens_st screen_water_year = {
    .up = NONE,
    .down = WATER_MONTH,
    .left = HOME,
    .right = NONE
    };
  adj_screens[HOME] = screen_home;
  adj_screens[PHOTO] = screen_photo;
  adj_screens[TIMER_READY] = screen_timer_ready;
  adj_screens[TIMER_ACTIVE] = screen_timer_active;
  adj_screens[TIMER_PAUSE] = screen_timer_pause;
  adj_screens[TIMER_DONE] = screen_timer_done;
  adj_screens[WATER_DAY] = screen_water_day;
  adj_screens[WATER_WEEK] = screen_water_week;
  adj_screens[WATER_MONTH] = screen_water_month;
  adj_screens[WATER_YEAR] = screen_water_year;


  // variable declarations
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  TTF_Font** fonts = NULL;

  // call initialization routine
  if (init(&window, &renderer, &fonts))
    return EXIT_FAILURE;

  // initialize state structure
  state_st state = {
    .current = HOME,
    .timer = 0
  };

  // execute main loop
  while (update(&state, adj_screens)) {
    render(&state, window, renderer, fonts);
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

