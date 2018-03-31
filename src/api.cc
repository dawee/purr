#include <iostream>

#include "api.h"
#include "game.h"
#include "job.h"
#include "texture.h"
#include "util.h"

namespace purr {
  APITexture * API::getTextureFromValue(v8::Local<v8::Value> value, const char * apiName) {
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

    v8::Local<v8::External> apiTextureWrap = v8::Local<v8::External>::Cast(textureObject->GetInternalField(1));
    APITexture * apiTexture = static_cast<APITexture *>(apiTextureWrap->Value());

    if (apiTexture == nullptr) {
      std::cerr << "Internal Purr Error: SDL texture has been somehow destroyed" << std::endl;
      return nullptr;
    }

    return apiTexture;
  }

  void API::onTextureObjectDestroyed(const v8::WeakCallbackInfo<APITexture> &data) {
    APITexture * apiTexture = data.GetParameter();
    DestroyTextureJob * job = new DestroyTextureJob(apiTexture->texture);

    apiTexture->api->worker->PushJob(static_cast<Job *>(job));
    delete apiTexture;
  }

  void API::getTextureWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    APITexture * apiTexture = getTextureFromValue(textureObject, "getTextureWidth");
    int width = 0;

    if (apiTexture != nullptr) {
      width = apiTexture->texture->Width();
    }

    info.GetReturnValue().Set(v8::Number::New(textureObject->GetIsolate(), width));
  }

  void API::getTextureHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    APITexture * apiTexture = getTextureFromValue(textureObject, "getTextureHeight");
    int height = 0;

    if (apiTexture != nullptr) {
      height = apiTexture->texture->Height();
    }

    info.GetReturnValue().Set(v8::Number::New(textureObject->GetIsolate(), height));
  }

  void API::getTextureLoadedState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    APITexture * apiTexture = getTextureFromValue(textureObject, "getTextureLoadedState");
    bool loaded = false;

    if (apiTexture != nullptr) {
      loaded = apiTexture->texture->IsLoaded();
    }

    info.GetReturnValue().Set(v8::Boolean::New(textureObject->GetIsolate(), loaded));
  }

  void API::loadTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no filename given to loadTexture" << std::endl;
      return;
    }

    API * api = static_cast<API *>(v8::Local<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
    v8::Isolate * isolate = info.This()->GetIsolate();
    std::string filename = ValueToSTDString(info[0]);
    Texture * texture = new Texture(filename);
    APITexture * apiTexture = new APITexture(api, texture);

    v8::Local<v8::ObjectTemplate> textureObjectTemplate = v8::ObjectTemplate::New(isolate);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "width"), &getTextureWidth);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "height"), &getTextureHeight);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "loaded"), &getTextureLoadedState);

    textureObjectTemplate->SetInternalFieldCount(2);

    v8::Local<v8::Object> textureObject = textureObjectTemplate->NewInstance();
    v8::Local<v8::Number> objectType = v8::Number::New(isolate, ObjectType::TEXTURE);
    v8::Persistent<v8::Object> pTextureObject;

    textureObject->SetInternalField(0, objectType);
    textureObject->SetInternalField(1, v8::External::New(isolate, apiTexture));

    pTextureObject.Reset(isolate, textureObject);
    pTextureObject.SetWeak(apiTexture, onTextureObjectDestroyed, v8::WeakCallbackType::kParameter);
    pTextureObject.MarkIndependent();

    LoadTextureJob * job = new LoadTextureJob(texture);

    api->worker->PushJob(static_cast<Job *>(job));
    info.GetReturnValue().Set(textureObject);
  }

  void API::drawTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no texture given to drawTexture" << std::endl;
      return;
    }

    APITexture * apiTexture = getTextureFromValue(info[0], "drawTexture");

    if (apiTexture != nullptr) {
      int x = info.Length() > 1 ? info[1]->Int32Value() : 0;
      int y = info.Length() > 2 ? info[2]->Int32Value() : 0;

      apiTexture->texture->Draw(Game::Instance()->Display(), x, y);
    }
  }

  API::API(v8::Isolate * isolate, Worker * worker) : Feeder::Feeder(isolate), worker(worker) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> apiTemplate = v8::ObjectTemplate::New(isolate);
    apiTemplate->SetInternalFieldCount(1);
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &loadTexture));
    apiTemplate->Set(v8::String::NewFromUtf8(isolate, "drawTexture"), v8::FunctionTemplate::New(isolate, &drawTexture));
    v8::Local<v8::Object> localRoot = apiTemplate->NewInstance();
    localRoot->SetInternalField(0, v8::External::New(isolate, this));


    root.Reset(isolate, localRoot);
  }

  APITexture::APITexture(API * api, Texture * texture) : api(api), texture(texture) {}
}
