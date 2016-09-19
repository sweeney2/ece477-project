#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// window information
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 900  //1280

// color information
#define FOREGROUND_COLOR {255, 255, 255}
#define BACKGROUND_COLOR {0, 0, 0}

// font information
#define FONT_FACE "Roboto-Regular.ttf"
#define FONT_SIZE_CLOCK 72
#define FONT_SIZE_QUOTE 36


int main(int argc, char** argv)
{
  // Initialize SDL. Exit on failure.
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(
        stderr,
        "Unable to initialize SDL: %s\n",
        SDL_GetError()
        );
    return EXIT_FAILURE;
  }

  // Initialize colors
  SDL_Color foreground_color = FOREGROUND_COLOR;
  SDL_Color background_color = BACKGROUND_COLOR;

  // Create a window. Exit on failure.
  SDL_Window *window = SDL_CreateWindow(
      "Magic Mirror",                           // title
      SDL_WINDOWPOS_CENTERED,                   // x position
      SDL_WINDOWPOS_CENTERED,                   // y position
      WINDOW_WIDTH,                             // width
      WINDOW_HEIGHT,                            // height
      SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS    // window flags
      );
  if (!window) {
    fprintf(
        stderr,
        "SDL_CreateWindow error: %s\n",
        SDL_GetError()
        );
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Create a renderer. Exit on failure.
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window,                                   // this renderer's window
      -1,                                       // rendering driver index
      0                                         // render flags
      );
  if (!renderer) {
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "SDL_CreateRenderer error: %s\n",
        SDL_GetError()
        );
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Initialize SDL_TTF
  if (TTF_Init() != 0) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "Unable to initialize SDL_TTF: %s\n",
        TTF_GetError()
        );
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Open TTF font for clock
  TTF_Font *clock_font = TTF_OpenFont(
      FONT_FACE,                                // font name
      FONT_SIZE_CLOCK                           // point size based on 72DPI
      );
  if (!clock_font) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "TTF_OpenFont error: %s\n",
        TTF_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Open TTF font for quote
  TTF_Font *quote_font = TTF_OpenFont(
      FONT_FACE,                                // font name
      FONT_SIZE_QUOTE                           // point size based on 72DPI
      );
  if (!quote_font) {
    TTF_CloseFont(clock_font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "TTF_OpenFont error: %s\n",
        TTF_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Render text using Shaded rendering
  SDL_Surface *clock_surface = TTF_RenderText_Shaded(
      clock_font,                               // TTF_Font
      "12:34 PM",                               // string to render
      foreground_color,                         // string color
      background_color                          // background color
      );
  if (!clock_surface) {
    TTF_CloseFont(clock_font);
    TTF_CloseFont(quote_font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "TTF_RenderText_Solid error: %s\n",
        TTF_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Render text using Shaded rendering
  SDL_Surface *quote_surface = TTF_RenderText_Shaded(
      quote_font,                               // TTF_Font
      "This is an example quote!",              // string to render
      foreground_color,                         // string color
      background_color                          // background color
      );
  if (!quote_surface) {
    TTF_CloseFont(clock_font);
    TTF_CloseFont(quote_font);
    SDL_FreeSurface(clock_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "TTF_RenderText_Solid error: %s\n",
        TTF_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // create texture from rendered surface
  SDL_Texture *clock_texture = SDL_CreateTextureFromSurface(
      renderer,                                 // renderer
      clock_surface                             // surface
      );
  if (!clock_surface) {
    TTF_CloseFont(clock_font);
    TTF_CloseFont(quote_font);
    SDL_FreeSurface(clock_surface);
    SDL_FreeSurface(quote_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "SDL_CreateTextureFromSurface error: %s\n",
        SDL_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // create texture from rendered surface
  SDL_Texture *quote_texture = SDL_CreateTextureFromSurface(
      renderer,                                 // renderer
      quote_surface                             // surface
      );
  if (!quote_surface) {
    TTF_CloseFont(clock_font);
    TTF_CloseFont(quote_font);
    SDL_FreeSurface(clock_surface);
    SDL_FreeSurface(quote_surface);
    SDL_DestroyTexture(clock_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fprintf(
        stderr,
        "SDL_CreateTextureFromSurface error: %s\n",
        SDL_GetError()
        );
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Basic render loop
  SDL_Rect clock_src_rect;
  SDL_Rect quote_src_rect;
  SDL_Rect clock_dst_rect;
  SDL_Rect quote_dst_rect;

  // error check this?
  TTF_SizeText(quote_font, "This is an example quote!", &quote_dst_rect.w, &quote_dst_rect.h);

  clock_src_rect.x = 0;
  quote_src_rect.x = 0;
  clock_src_rect.y = 0;
  quote_src_rect.y = 0;
  clock_src_rect.w = clock_surface->w;
  quote_src_rect.w = quote_surface->w;
  clock_src_rect.h = clock_surface->h;
  quote_src_rect.h = quote_surface->h;
  clock_dst_rect.x = 20;
  quote_dst_rect.x = (WINDOW_WIDTH - quote_dst_rect.w) / 2;
  clock_dst_rect.y = 20;
  quote_dst_rect.y = WINDOW_HEIGHT - quote_dst_rect.h - 10;
  clock_dst_rect.w = clock_surface->w;
  quote_dst_rect.w = quote_surface->w;
  clock_dst_rect.h = clock_surface->h;
  quote_dst_rect.h = quote_surface->h;

  for (int i = 0; i < 6; i++) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, clock_texture, &clock_src_rect, &clock_dst_rect);
    SDL_RenderCopy(renderer, quote_texture, &quote_src_rect, &quote_dst_rect);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
  }

  // Clean up
  TTF_CloseFont(clock_font);
  TTF_CloseFont(quote_font);
  TTF_Quit();
  SDL_FreeSurface(clock_surface);
  SDL_FreeSurface(quote_surface);
  SDL_DestroyTexture(clock_texture);
  SDL_DestroyTexture(quote_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}

