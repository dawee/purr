#include <iostream>

#include <SDL2/SDL.h>
#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "project.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char * argv[]) {
  if (argc != 2) {
    printf("usage:\n\tpurr <filename>\n");
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
    "Purr",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  );

  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL){
  	SDL_DestroyWindow(window);
  	std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
  	SDL_Quit();
  	return 1;
  }

  SDL_SetWindowBordered(window, SDL_TRUE);
  SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
  SDL_UpdateWindowSurface(window);

  filesystem::path filePath(argv[1]);

  v8::Platform* platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    purr::Module * main = purr::Project::Instance()->SaveModule(filePath.make_absolute().str());

    SDL_Event event;
    bool quit = false;

    while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          SDL_HideWindow(window);
          quit = true;
        }
      }

      main->CallExportedFunction("update");
      main->CallExportedFunction("draw");
    }
  }

  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
