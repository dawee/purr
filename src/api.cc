#include <iostream>

#include "api.h"
#include "project.h"
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

  void API::DrawTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no texture given to drawTexture" << std::endl;
      return;
    }

    if (!info[0]->IsObject()) {
      std::cerr << "Error: given argument to drawTexture is not an object" << std::endl;
      return;
    }

    v8::Local<v8::Object> textureObject = info[0]->ToObject();

    if (textureObject->InternalFieldCount() != 2) {
      std::cerr << "Error: given argument to drawTexture is not a Purr native object" << std::endl;
      return;
    }

    v8::Isolate * isolate = info.This()->GetIsolate();

    if (textureObject->GetInternalField(0)->ToNumber(isolate)->Int32Value() != ObjectType::TEXTURE) {
      std::cerr << "Error: given argument to drawTexture is not a texture" << std::endl;
      return;
    }

    v8::Local<v8::External> textureWrap = v8::Local<v8::External>::Cast(textureObject->GetInternalField(1));
    Texture * texture = static_cast<Texture *>(textureWrap->Value());

    if (texture == nullptr) {
      std::cerr << "Internal Purr Error: SDL texture has been somehow destroyed" << std::endl;
      return;
    }

    texture->Draw(Project::Instance()->Display());
  }

  API::API(v8::Isolate* isolate) : Feeder::Feeder(isolate) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> apiTemplate = v8::ObjectTemplate::New(isolate);
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &API::LoadTexture));
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "drawTexture"), v8::FunctionTemplate::New(isolate, &API::DrawTexture));
    v8::Local<v8::Object> localRoot = apiTemplate->NewInstance();

    root.Reset(isolate, localRoot);
  }
}
