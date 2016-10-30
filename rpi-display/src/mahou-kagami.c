#include "mahou-kagami.h"

int
init (config_t* config, SDL_Window** window, SDL_Renderer** renderer, SDL_Texture*** assets, TTF_Font*** fonts)
{
  // TODO: Add destroys in the proper order

  // asset path
  const char* asset_path;
  if (config_lookup_string(config, "magic.asset_path", &asset_path) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_string failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }

  // font settings
  const char* roboto_file;
  if (config_lookup_string(config, "magic.fonts.roboto", &roboto_file) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_string failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }
  fontInfo_st* font_info = (fontInfo_st*) malloc(FONT_COUNT * sizeof(fontInfo_st));

  char* roboto_path = (char*) malloc(strlen(asset_path) + strlen(roboto_file) + 1);
  strcpy(roboto_path, asset_path);
  strcat(roboto_path, roboto_file);

  fontInfo_st font_debug = {
    .path = roboto_path,
    .size = 20
    };
  fontInfo_st font_clock = {
    .path = roboto_path,
    .size = 120
    };
  fontInfo_st font_quote = {
    .path = roboto_path,
    .size = 40
    };
  font_info[DEBUG] = font_debug;
  font_info[CLOCK] = font_clock;
  font_info[QUOTE] = font_quote;

  // asset settings
  const char* usb_file;
  if (config_lookup_string(config, "magic.assets.usb", &usb_file) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_string failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }
  assetInfo_st* asset_info = (assetInfo_st*) malloc(ASSET_COUNT * sizeof(assetInfo_st));

  char* usb_path = (char*) malloc(strlen(asset_path) + strlen(usb_file) + 1);
  strcpy(usb_path, asset_path);
  strcat(usb_path, usb_file);

  assetInfo_st asset_usb = {
    .path = usb_path
    };
  asset_info[ASSET_USB] = asset_usb;


  // window settings
  int x_pos, y_pos, width, height;
  if (config_lookup_int(config, "magic.window.x_pos", &x_pos) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_int failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }

  if (config_lookup_int(config, "magic.window.y_pos", &y_pos) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_int failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }

  if (config_lookup_int(config, "magic.window.width", &width) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_int failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }

  if (config_lookup_int(config, "magic.window.height", &height) == CONFIG_FALSE) {
    fprintf(
        stderr,
        "config_lookup_int failed: %s\n",
        config_error_text(config)
        );
    return -1;
  }

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
      x_pos,
      y_pos,
      width,
      height,
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

  // check asset paths
  for (int i = 0; i < ASSET_COUNT; i++) {
    if (access(asset_info[i].path, F_OK) == -1) {
      fprintf(
          stderr,
          "[asset %d/%d] asset not found: %s\n",
          i + 1,
          ASSET_COUNT,
          asset_info[i].path
          );
      SDL_DestroyRenderer(*renderer);
      SDL_DestroyWindow(*window);
      TTF_Quit();
      SDL_Quit();
      free(asset_info);
      return -1;
    }
  }

  // allocate space for assets
  *assets = (SDL_Texture**) malloc(ASSET_COUNT * sizeof(**assets));

  // create asset objects
  SDL_Surface* temp_surface;
  for (int i = 0; i < ASSET_COUNT; i++) {
    temp_surface = SDL_LoadBMP(
        asset_info[i].path
        );
    *(*assets + i) = SDL_CreateTextureFromSurface(
      *renderer,
      temp_surface
      );
    SDL_FreeSurface(temp_surface);
    if (!*(*assets + i)) {
      fprintf(
          stderr,
          "[asset %d/%d] SDL_CreateTextureFromSurface failed: %s\n",
          i + 1,
          ASSET_COUNT,
          SDL_GetError()
          );
      for (int j = 0; j < i; j++) {
        SDL_DestroyTexture(*(*assets + j));
      }
      free(*assets);
      SDL_DestroyRenderer(*renderer);
      SDL_DestroyWindow(*window);
      TTF_Quit();
      SDL_Quit();
      free(asset_info);
      return -1;
    }
  }


  free(font_info);
  free(asset_info);
  return 0;
}



int
main(int argc, char* argv[])
{
  // option flags
  int config_flag = 0;

  config_t config;
  config_init(&config);

  while (argc > 1) {
    if (!strcmp(argv[1], "--config")) {
      if (config_read_file(&config, argv[2]) == CONFIG_FALSE) {
        fprintf(
            stderr,
            "config_read_file failed: %s\n",
            config_error_text(&config)
            );
        config_destroy(&config);
        return EXIT_FAILURE;
      }
      config_flag = 1;
      argv += 2;
      argc -= 2;
    } else {
      fprintf(stderr, "invalid arguments\n");
      return EXIT_FAILURE;
    }
  }

  // default config
  if (!config_flag) {
    if (config_read_file(&config, CONFIG_FILE) == CONFIG_FALSE) {
      fprintf(
          stderr,
          "config_read_file failed: %s\n",
          config_error_text(&config)
          );
      config_destroy(&config);
      return EXIT_FAILURE;
    }
  }

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
  SDL_Texture** assets = NULL;
  TTF_Font** fonts = NULL;

  // call initialization routine
  if (init(&config, &window, &renderer, &assets, &fonts))
    return EXIT_FAILURE;

  // initialize state structure
  state_st state = {
    .current = HOME,
    .timer = 0
  };

  // execute main loop
  while (update(&state, adj_screens)) {
    render(&state, window, renderer, assets, fonts);
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
  config_destroy(&config);

  return EXIT_SUCCESS;
}

