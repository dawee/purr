#include <iostream>

#include "display.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace purr {
  Display::Display() {
    window = SDL_CreateWindow(
      "Purr",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
      std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
      return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr){
      SDL_DestroyWindow(window);
      std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
      SDL_Quit();
      return;
    }

    SDL_Surface * surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

    if (surface == nullptr){
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      std::cerr << "SDL_GetWindowSurface Error: " << SDL_GetError() << std::endl;
      SDL_Quit();
      return;
    }

    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    background = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_SetWindowBordered(window, SDL_TRUE);
  }

  Display::~Display() {
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void Display::Draw() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  void Display::Hide() {
    SDL_HideWindow(window);
  }
}
