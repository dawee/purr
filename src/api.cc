#include <iostream>

#include "api.h"
#include "texture.h"
#include "util.h"

namespace purr {
  void API::LoadTexture(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() == 0) {
      std::cerr << "Error: no filename given to loadTexture" << std::endl;
      return;
    }

    std::string filename = ValueToSTDString(args[0]);
    Texture * texture = new Texture(filename);

    texture->Load();
  }

  API::API(v8::Isolate* isolate) : Feeder::Feeder(isolate) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> apiTemplate = v8::ObjectTemplate::New(isolate);
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &API::LoadTexture));
    v8::Local<v8::Object> localRoot = apiTemplate->NewInstance();

    root.Reset(isolate, localRoot);
  }
}