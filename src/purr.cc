#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <SDL2/SDL.h>
#include <v8/libplatform/libplatform.h>
#include <v8/v8.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage:\n\tpurr <filename>\n");
    return 1;
  }

  ifstream scriptFile;

  scriptFile.open(argv[1]);

  if (!scriptFile.is_open()) {
    printf("failed to read '%s'\n", argv[1]);
    return 1;
  }

  string script;

  scriptFile.seekg(0, std::ios::end);
  script.reserve(scriptFile.tellg());
  scriptFile.seekg(0, std::ios::beg);
  script.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
  scriptFile.close();


  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }


  window = SDL_CreateWindow(
 			    argv[1],
 			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
 			    SCREEN_WIDTH, SCREEN_HEIGHT,
 			    SDL_WINDOW_SHOWN
 			    );

  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);

  v8::Platform* platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
  v8::ArrayBuffer::Allocator::NewDefaultAllocator();

  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate, script.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(isolate, result);

    printf("script result: %s\n", *utf8);
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;

  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
