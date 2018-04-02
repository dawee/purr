#include <iostream>
#include <SDL2/SDL_image.h>

#include "display.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace purr {
  SDLDisplay::SDLDisplay() {
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

    SDL_Surface * surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);

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

  SDL_Texture * SDLDisplay::CreateSDLTextureFromImage(std::string filename) {
    SDL_Surface * surface = IMG_Load(filename.c_str());

    if (surface == nullptr) {
      std::cout << "IMG_Load Error: " << SDL_GetError() << std::endl;
      return nullptr;
    }

    SDL_Texture * sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (sdlTexture == nullptr){
    	std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    	return nullptr;
    }

    return sdlTexture;
  }

  SDLDisplay::~SDLDisplay() {
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void SDLDisplay::Render() {
    SDL_RenderPresent(renderer);
  }

  void SDLDisplay::Clear() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
  }

  void SDLDisplay::Hide() {
    SDL_HideWindow(window);
  }

  void SDLDisplay::DrawSDLTexture(SDL_Texture * texture, SDL_Rect& src, SDL_Rect& dest) {
    SDL_RenderCopy(renderer, texture, &src, &dest);
  }
}
