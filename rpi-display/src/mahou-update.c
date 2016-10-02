#include "mahou-update.h"

int
update(state_st* state)
{
  SDL_Event event;

  // grab polled events and process them
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (state->current == HOME) {
            state->current = PHOTO;
          } else if (state->current == PHOTO) {
            //state->current = ;
          } else if (state->current == TIMER_READY) {
            state->current = TIMER_ACTIVE;
          } else if (state->current == TIMER_ACTIVE) {
            //state->current = ;
          } else if (state->current == TIMER_PAUSE) {
            state->current = TIMER_ACTIVE;
          } else if (state->current == TIMER_DONE) {
            state->current = TIMER_ACTIVE;
          } else if (state->current == WATER_DAY) {
            state->current = WATER_WEEK;
          } else if (state->current == WATER_WEEK) {
            state->current = WATER_MONTH;
          } else if (state->current == WATER_MONTH) {
            state->current = WATER_YEAR;
          } else if (state->current == WATER_YEAR) {
            //state->current = ;
          }
          break;
        case SDLK_DOWN:
          if (state->current == HOME) {
            //state->current = ;
          } else if (state->current == PHOTO) {
            state->current = HOME;
          } else if (state->current == TIMER_READY) {
            //state->current = ;
          } else if (state->current == TIMER_ACTIVE) {
            state->current = TIMER_PAUSE;
          } else if (state->current == TIMER_PAUSE) {
            //state->current = ;
          } else if (state->current == TIMER_DONE) {
            //state->current = ;
          } else if (state->current == WATER_DAY) {
            //state->current = ;
          } else if (state->current == WATER_WEEK) {
            state->current = WATER_DAY;
          } else if (state->current == WATER_MONTH) {
            state->current = WATER_WEEK;
          } else if (state->current == WATER_YEAR) {
            state->current = WATER_MONTH;
          }
          break;
        case SDLK_LEFT:
          if (state->current == HOME) {
            state->current = TIMER_READY;
          } else if (state->current == PHOTO) {
            //state->current = ;
          } else if (state->current == TIMER_READY) {
            //state->current = ;
          } else if (state->current == TIMER_ACTIVE) {
            //state->current = ;
          } else if (state->current == TIMER_PAUSE) {
            state->current = TIMER_READY;
          } else if (state->current == TIMER_DONE) {
            state->current = TIMER_READY;
          } else if (state->current == WATER_DAY) {
            state->current = HOME;
          } else if (state->current == WATER_WEEK) {
            state->current = HOME;
          } else if (state->current == WATER_MONTH) {
            state->current = HOME;
          } else if (state->current == WATER_YEAR) {
            state->current = HOME;
          }
          break;
        case SDLK_RIGHT:
          if (state->current == HOME) {
            state->current = WATER_DAY;
          } else if (state->current == PHOTO) {
            //state->current = ;
          } else if (state->current == TIMER_READY) {
            state->current = HOME;
          } else if (state->current == TIMER_ACTIVE) {
            //state->current = ;
          } else if (state->current == TIMER_PAUSE) {
            //state->current = ;
          } else if (state->current == TIMER_DONE) {
            state->current = HOME;
          } else if (state->current == WATER_DAY) {
            //state->current = ;
          } else if (state->current == WATER_WEEK) {
            //state->current = ;
          } else if (state->current == WATER_MONTH) {
            //state->current = ;
          } else if (state->current == WATER_YEAR) {
            //state->current = ;
          }
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

