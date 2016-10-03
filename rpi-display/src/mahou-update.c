#include "mahou-update.h"

int
update(state_st* state, adjScreens_st* adj_states)
{
  SDL_Event event;

  // grab polled events and process them
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      adjScreens_st* adj_states_polled = (adj_states + state->current);
      switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (adj_states_polled->up != NONE)
            state->current = adj_states_polled->up;
          break;
        case SDLK_DOWN:
          if (adj_states_polled->down != NONE)
            state->current = adj_states_polled->down;
          break;
        case SDLK_LEFT:
          if (adj_states_polled->left != NONE)
            state->current = adj_states_polled->left;
          break;
        case SDLK_RIGHT:
          if (adj_states_polled->right != NONE)
            state->current = adj_states_polled->right;
          break;
        case SDLK_z:
          break;
        case SDLK_x:
          break;
        case SDLK_ESCAPE:
          return 0;
          break;
        default:
          break;
      }
    }
  }
  return 1;
}

