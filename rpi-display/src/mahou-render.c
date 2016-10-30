#include "mahou-render.h"

int
render(state_st* state, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture** assets, TTF_Font** fonts)
{
  GSList* texture_list;
  GSList* iter;

  SDL_Color white = {
      .r = 255,
      .g = 255,
      .b = 255
      };
  SDL_Surface* temp_surface = NULL;
  SDL_Texture* temp_texture = NULL;
  texture_st* ts = NULL;

  // state-specific rendering
  switch (state->current) {
    case HOME:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "HOME",
          white
          );
      break;
    case PHOTO:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "PHOTO",
          white
          );
      break;
    case TIMER_READY:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_READY",
          white
          );
      break;
    case TIMER_ACTIVE:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_ACTIVE",
          white
          );
      break;
    case TIMER_PAUSE:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_PAUSE",
          white
          );
      break;
    case TIMER_DONE:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "TIMER_DONE",
          white
          );
      break;
    case WATER_DAY:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_DAY",
          white
          );
      break;
    case WATER_WEEK:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_WEEK",
          white
          );
      break;
    case WATER_MONTH:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_MONTH",
          white
          );
      break;
    case WATER_YEAR:
      temp_surface = TTF_RenderText_Solid(
          *(fonts + DEBUG),
          "WATER_YEAR",
          white
          );
      break;
    default:
      break;
  }

  temp_texture = SDL_CreateTextureFromSurface(
      renderer,
      temp_surface
      );
  if (!temp_texture) {
    fprintf(
        stderr,
        "SDL_CreateTextureFromSurface failed: %s\n",
        SDL_GetError()
        );
    SDL_FreeSurface(temp_surface);
    return -1;
  }

  ts = (texture_st*) malloc(sizeof(texture_st));
  ts->texture = temp_texture;
  ts->rect.x = 4;
  ts->rect.y = 4;
  ts->rect.w = 0;
  ts->rect.h = 0;
  texture_list = g_slist_append(texture_list, &ts);

  // state-agnostic rendering
  if (state->usb == USB_IDLE) {
    ts = (texture_st*) malloc(sizeof(texture_st));
    ts->texture = *(assets + ASSET_USB);
    ts->rect.x = 50;
    ts->rect.y = 50;
    ts->rect.w = 113;
    ts->rect.h = 50;
    texture_list = g_slist_append(texture_list, &ts);
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
    SDL_FreeSurface(temp_surface);
    SDL_DestroyTexture(temp_texture);
    return -1;
  }

  SDL_RenderClear(renderer);
  for (iter = texture_list; iter; iter = iter->next) {
    ts = iter->data;
    SDL_RenderCopy(renderer, ts->texture, NULL, &(ts->rect));
  }
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(temp_texture);

  return 0;
}

