#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <SDL2/SDL.h>
#include <v8/libplatform/libplatform.h>
#include <v8/v8.h>
#include <filesystem/path.h>
#include <filesystem/resolver.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace std;

class ExportsHolder {

  private:
    ExportsHolder() {}

    std::map<std::string, v8::Persistent<v8::Value> *> savedExports;

    static ExportsHolder * instance;

  public:
    static ExportsHolder * Instance() {
      if (instance == NULL) {
        instance = new ExportsHolder();
      }

      return instance;
    }

    v8::Persistent<v8::Value> * getExports(v8::Isolate * isolate, std::string name) {
      if (savedExports.count(name) == 0) {
        v8::Local<v8::Object> exports = v8::Object::New(isolate);
        savedExports[name] = new v8::Persistent<v8::Value>(isolate, exports);
      }

      return savedExports[name];
    }

    void setExports(v8::Isolate * isolate, std::string name, v8::Local<v8::Value> exports) {
      if (savedExports.count(name) > 0) {
        savedExports[name]->Reset();
      }

      savedExports[name] = new v8::Persistent<v8::Value>(isolate, exports);
    }

};

ExportsHolder * ExportsHolder::instance = NULL;

std::string ValueToSTDString(v8::Local<v8::Value> value) {
  v8::Local<v8::String> castedValue = value->ToString();
  v8::String::Utf8Value utf8Value(castedValue);
  std::string strValue(*utf8Value);

  return strValue;
}

void ExportsGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Object> module = info.Holder();

  std::string fileNameStr = ValueToSTDString(module->Get(v8::String::NewFromUtf8(module->GetIsolate(), "__filename__")));
  v8::Persistent<v8::Value> * exports = ExportsHolder::Instance()->getExports(module->GetIsolate(), fileNameStr);
  info.GetReturnValue().Set(*exports);
}

void ExportsSetter(v8::Local<v8::String > property, v8::Local<v8::Value > value, const v8::PropertyCallbackInfo<void> &info) {
  v8::Local<v8::Object> module = info.Holder();

  std::string fileNameStr = ValueToSTDString(module->Get(v8::String::NewFromUtf8(module->GetIsolate(), "__filename__")));
  ExportsHolder::Instance()->setExports(module->GetIsolate(), fileNameStr, value);
}

void ModuleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Object> module = info.Holder();
  info.GetReturnValue().Set(module);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage:\n\tpurr <filename>\n");
    return 1;
  }

  ifstream scriptFile;

  filesystem::path filePath(argv[1]);

  scriptFile.open(argv[1]);

  if (!scriptFile.is_open()) {
    printf("failed to read '%s'\n", argv[1]);
    return 1;
  }

  string scriptSource;

  scriptFile.seekg(0, std::ios::end);
  scriptSource.reserve(scriptFile.tellg());
  scriptFile.seekg(0, std::ios::beg);
  scriptSource.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
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

  SDL_SetWindowBordered(window, SDL_TRUE);
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
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

    v8::Local<v8::ObjectTemplate> moduleTemplate = v8::ObjectTemplate::New(isolate);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "exports"), &ExportsGetter, &ExportsSetter);
    moduleTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "module"), &ModuleGetter);

    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, moduleTemplate);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> filename = v8::String::NewFromUtf8(
      isolate,
      filePath.make_absolute().str().c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();


    context->Global()->Set(v8::String::NewFromUtf8(isolate, "__filename__"), filename);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(
      isolate,
      scriptSource.c_str(),
      v8::NewStringType::kNormal
    ).ToLocalChecked();
    v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(isolate, result);
    SDL_Event event;
    bool quit = false;

    while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
          SDL_HideWindow(window);
          quit = true;
        }
      }
    }

    printf("script result: %s\n", *utf8);
  }

  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
