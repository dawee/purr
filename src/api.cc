#include <iostream>

#include "api.h"
#include "texture.h"
#include "util.h"

namespace purr {
  void API::LoadTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no filename given to loadTexture" << std::endl;
      return;
    }

    v8::Isolate * isolate = info.This()->GetIsolate();
    std::string filename = ValueToSTDString(info[0]);
    Texture * texture = new Texture(filename);

    v8::Local<v8::ObjectTemplate> textureObjectTemplate = v8::ObjectTemplate::New(isolate);
    textureObjectTemplate->SetInternalFieldCount(2);

    v8::Local<v8::Object> textureObject = textureObjectTemplate->NewInstance();
    v8::Local<v8::Number> objectType = v8::Number::New(isolate, ObjectType::TEXTURE);

    textureObject->SetInternalField(0, objectType);
    textureObject->SetInternalField(1, v8::External::New(isolate, texture));

    texture->Load();

    info.GetReturnValue().Set(textureObject);
  }

  API::API(v8::Isolate* isolate) : Feeder::Feeder(isolate) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> apiTemplate = v8::ObjectTemplate::New(isolate);
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &API::LoadTexture));
    v8::Local<v8::Object> localRoot = apiTemplate->NewInstance();

    root.Reset(isolate, localRoot);
  }
}
