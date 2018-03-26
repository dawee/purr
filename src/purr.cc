#include <iostream>

#include <SDL2/SDL.h>
#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#include "project.h"

int main(int argc, char * argv[]) {
  if (argc != 2) {
    printf("usage:\n\tpurr <filename>\n");
    return 1;
  }


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
    purr::Project * project = purr::Project::CreateInstance();
    purr::Module * main = project->SaveModule(filePath.make_absolute().str());

    SDL_Event event;
    bool quit = false;

    while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          project->Display()->Hide();
          quit = true;
        }
      }

      main->CallExportedFunction("update");
      project->Display()->Clear();
      main->CallExportedFunction("draw");
      project->Display()->Render();
    }
  }

  delete create_params.array_buffer_allocator;
  purr::Project::DeleteInstance();
  return 0;
}
