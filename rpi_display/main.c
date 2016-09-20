#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>


// window information
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 900

// font information
#define FONT_FACE "Roboto-Regular.ttf"
#define FONT_SIZE_CLOCK 72


char *gettime(void)
{
  char        * result        ;
  time_t        raw_time      ;
  struct tm   * timeinfo      ;
  char          ampm[3] = "AM";

  time(&raw_time);
  timeinfo = localtime(&raw_time);

  // set AM/PM
  if (timeinfo->tm_hour > 11) {
    ampm[0] = 'P';
  }

  result = (char*) malloc(10 * sizeof(char));
  sprintf(
      result,
      "%d:%02d %s",
      (timeinfo->tm_hour + 11) % 12 + 1,
      timeinfo->tm_min,
      ampm
      );
  return result;
}

int main(int argc, char** argv)
{
  // variable declarations
  SDL_Window    * window                  = NULL;
  SDL_Renderer  * renderer                = NULL;
  SDL_Surface   * clock_surface           = NULL;
  SDL_Surface   * clock_surface_next      = NULL;
  SDL_Texture   * clock_texture           = NULL;
  SDL_Rect        clock_rect                    ;
  SDL_Color       foreground_color              ;
  SDL_Color       background_color              ;
  TTF_Font      * clock_font              = NULL;
  char          * time_current            = NULL;
  char          * time_last               = NULL;

  // initialize colors
  foreground_color.r = 255;
  foreground_color.g = 255;
  foreground_color.b = 255;
  background_color.r = 0;
  background_color.g = 0;
  background_color.b = 0;

  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  // create a window. Exit on failure.
  window = SDL_CreateWindow(
      "Magic Mirror",                           // title
      SDL_WINDOWPOS_CENTERED,                   // x position
      SDL_WINDOWPOS_CENTERED,                   // y position
      WINDOW_WIDTH,                             // width
      WINDOW_HEIGHT,                            // height
      SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS    // window flags
      );

  // create a renderer. Exit on failure.
  renderer = SDL_CreateRenderer(
      window,                                   // this renderer's window
      -1,                                       // rendering driver index
      0                                         // render flags
      );

  // initialize SDL_TTF
  TTF_Init();

  // open TTF font for clock
  clock_font = TTF_OpenFont(
      FONT_FACE,                                // font name
      FONT_SIZE_CLOCK                           // point size based on 72DPI
      );

  // get time
  time_last = (char*) malloc(30 * sizeof(char));
  strcpy(time_last, "");
  time_current = gettime();

  SDL_Event event;
  int loop = 1;

  while (loop) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        loop = 0;
      }
    }
    free(time_current);
    time_current = gettime();
    if (strcmp(time_current, time_last)) {
      // render text using Shaded rendering
      printf("rerendering screen\n");
      clock_surface = TTF_RenderText_Shaded(
          clock_font,                               // TTF_Font
          time_current,                             // string to render
          foreground_color,                         // string color
          background_color                          // background color
          );
      clock_texture = SDL_CreateTextureFromSurface(
          renderer,                                 // renderer
          clock_surface                             // surface
          );
      clock_rect.w = clock_surface->w;
      clock_rect.h = clock_surface->h;
      clock_rect.x = 20;
      clock_rect.y = 20;

      SDL_FreeSurface(clock_surface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, clock_texture, NULL, &clock_rect);
      SDL_RenderPresent(renderer);

      SDL_DestroyTexture(clock_texture);
      strcpy(time_last, time_current);
    } else {
//    printf("Nothing to render: date strings differ (%s) vs (%s)\n",
//        time_current,
//        time_last
//        );
    }
  }

  // Clean up
  free(time_current);
  free(time_last);
  TTF_CloseFont(clock_font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}

