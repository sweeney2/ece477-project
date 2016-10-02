#include "mahou-render.h"

int
render(state_st* state, SDL_Window* window, SDL_Renderer* renderer, TTF_Font** fonts)
{
  SDL_Color white = {
      .r = 255,
      .g = 255,
      .b = 255
      };
  SDL_Surface* debug_surface = NULL;
  SDL_Texture* debug_texture = NULL;

  switch (state->current) {
    case HOME:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "HOME",
          white
          );
      break;
    case PHOTO:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "PHOTO",
          white
          );
      break;
    case TIMER_READY:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_READY",
          white
          );
      break;
    case TIMER_ACTIVE:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_ACTIVE",
          white
          );
      break;
    case TIMER_PAUSE:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_PAUSE",
          white
          );
      break;
    case TIMER_DONE:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_DONE",
          white
          );
      break;
    case WATER_DAY:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_DAY",
          white
          );
      break;
    case WATER_WEEK:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_WEEK",
          white
          );
      break;
    case WATER_MONTH:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_MONTH",
          white
          );
      break;
    case WATER_YEAR:
      debug_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_YEAR",
          white
          );
      break;
    default:
      break;
  }

  debug_texture = SDL_CreateTextureFromSurface(
      renderer,
      debug_surface
      );
  if (!debug_texture) {
    fprintf(
        stderr,
        "SDL_CreateTextureFromSurface failed: %s\n",
        SDL_GetError()
        );
    SDL_FreeSurface(debug_surface);
    return -1;
  }

  // get size of renderer
  int renderer_w = 0;
  int renderer_h = 0;
  if (SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h)) {
    fprintf(
        stderr,
        "SDL_GetRendererOutputSize failed: %s\n",
        SDL_GetError()
        );
    SDL_FreeSurface(debug_surface);
    SDL_DestroyTexture(debug_texture);
    return -1;
  }

  SDL_Rect debug_rect = {
    .x = renderer_w - debug_surface->w - 4,
    .y = 2,
    .w = debug_surface->w,
    .h = debug_surface->h
  };
  SDL_FreeSurface(debug_surface);

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, debug_texture, NULL, &debug_rect);
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(debug_texture);

  return 0;
}

