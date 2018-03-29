#include <iostream>

#include "api.h"
#include "game.h"
#include "job.h"
#include "texture.h"
#include "util.h"

namespace purr {
  static Texture * getTextureFromValue(v8::Local<v8::Value> value, const char * apiName) {
    if (!value->IsObject()) {
      std::cerr << "Error: given argument to " << apiName << " is not an object" << std::endl;
      return nullptr;
    }

    v8::Local<v8::Object> textureObject = value->ToObject();

    if (textureObject->InternalFieldCount() != 2) {
      std::cerr << "Error: given argument to " << apiName << " is not a Purr native object" << std::endl;
      return nullptr;
    }

    v8::Isolate * isolate = textureObject->GetIsolate();

    if (textureObject->GetInternalField(0)->ToNumber(isolate)->Int32Value() != ObjectType::TEXTURE) {
      std::cerr << "Error: given argument to " << apiName << " is not a texture" << std::endl;
      return nullptr;
    }

    v8::Local<v8::External> textureWrap = v8::Local<v8::External>::Cast(textureObject->GetInternalField(1));
    Texture * texture = static_cast<Texture *>(textureWrap->Value());

    if (texture == nullptr) {
      std::cerr << "Internal Purr Error: SDL texture has been somehow destroyed" << std::endl;
      return nullptr;
    }

    return texture;
  }

  static void onTextureObjectDestroyed(const v8::WeakCallbackInfo<Texture> &data) {
    Texture * texture = data.GetParameter();
    DestroyTextureJob * job = new DestroyTextureJob(texture);

    Game::Instance()->PushJob(static_cast<Job *>(job));
  }

  static void getTextureWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    Texture * texture = getTextureFromValue(textureObject, "getTextureWidth");
    int width = 0;

    if (texture != nullptr) {
      width = texture->Width();
    }

    info.GetReturnValue().Set(v8::Number::New(textureObject->GetIsolate(), width));
  }

  static void loadTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no filename given to loadTexture" << std::endl;
      return;
    }

    v8::Isolate * isolate = info.This()->GetIsolate();
    std::string filename = ValueToSTDString(info[0]);
    Texture * texture = new Texture(filename);

    v8::Local<v8::ObjectTemplate> textureObjectTemplate = v8::ObjectTemplate::New(isolate);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "width"), &getTextureWidth);

    textureObjectTemplate->SetInternalFieldCount(2);

    v8::Local<v8::Object> textureObject = textureObjectTemplate->NewInstance();
    v8::Local<v8::Number> objectType = v8::Number::New(isolate, ObjectType::TEXTURE);
    v8::Persistent<v8::Object> pTextureObject;

    textureObject->SetInternalField(0, objectType);
    textureObject->SetInternalField(1, v8::External::New(isolate, texture));

    pTextureObject.Reset(isolate, textureObject);
    pTextureObject.SetWeak(texture, onTextureObjectDestroyed, v8::WeakCallbackType::kParameter);
    pTextureObject.MarkIndependent();

    LoadTextureJob * job = new LoadTextureJob(texture);

    Game::Instance()->PushJob(static_cast<Job *>(job));
    info.GetReturnValue().Set(textureObject);
  }

  static void drawTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no texture given to drawTexture" << std::endl;
      return;
    }

    Texture * texture = getTextureFromValue(info[0], "drawTexture");

    if (texture != nullptr) {
      int x = info.Length() > 1 ? info[1]->Int32Value() : 0;
      int y = info.Length() > 2 ? info[2]->Int32Value() : 0;

      texture->Draw(Game::Instance()->Display(), x, y);
    }
  }

  API::API(v8::Isolate* isolate) : Feeder::Feeder(isolate) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> apiTemplate = v8::ObjectTemplate::New(isolate);
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &loadTexture));
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "drawTexture"), v8::FunctionTemplate::New(isolate, &drawTexture));
    v8::Local<v8::Object> localRoot = apiTemplate->NewInstance();

    root.Reset(isolate, localRoot);
  }
}
