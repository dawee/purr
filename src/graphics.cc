#include <iostream>

#include "graphics.h"
#include "engine.h"
#include "job.h"
#include "texture.h"
#include "util.h"

namespace purr {
  GraphicsTexture * Graphics::getTextureFromValue(v8::Local<v8::Value> value, const char * graphicsName) {
    if (!value->IsObject()) {
      std::cerr << "Error: given argument to " << graphicsName << " is not an object" << std::endl;
      return nullptr;
    }

    v8::Local<v8::Object> textureObject = value->ToObject();

    if (textureObject->InternalFieldCount() != 2) {
      std::cerr << "Error: given argument to " << graphicsName << " is not a Purr native object" << std::endl;
      return nullptr;
    }

    v8::Isolate * isolate = textureObject->GetIsolate();

    if (textureObject->GetInternalField(0)->ToNumber(isolate)->Int32Value() != ObjectType::TEXTURE) {
      std::cerr << "Error: given argument to " << graphicsName << " is not a texture" << std::endl;
      return nullptr;
    }

    v8::Local<v8::External> graphicsTextureWrap = v8::Local<v8::External>::Cast(textureObject->GetInternalField(1));
    GraphicsTexture * graphicsTexture = static_cast<GraphicsTexture *>(graphicsTextureWrap->Value());

    if (graphicsTexture == nullptr) {
      std::cerr << "Internal Purr Error: SDL texture has been somehow destroyed" << std::endl;
      return nullptr;
    }

    return graphicsTexture;
  }

  void Graphics::onTextureObjectDestroyed(const v8::WeakCallbackInfo<GraphicsTexture> &data) {
    GraphicsTexture * graphicsTexture = data.GetParameter();
    DestroyTextureJob * job = new DestroyTextureJob(graphicsTexture->texture);

    graphicsTexture->graphics->worker->PushJob(static_cast<Job *>(job));
    delete graphicsTexture;
  }

  void Graphics::getTextureWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    GraphicsTexture * graphicsTexture = getTextureFromValue(textureObject, "getTextureWidth");
    int width = 0;

    if (graphicsTexture != nullptr) {
      width = graphicsTexture->texture->Width();
    }

    info.GetReturnValue().Set(v8::Number::New(textureObject->GetIsolate(), width));
  }

  void Graphics::getTextureHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    GraphicsTexture * graphicsTexture = getTextureFromValue(textureObject, "getTextureHeight");
    int height = 0;

    if (graphicsTexture != nullptr) {
      height = graphicsTexture->texture->Height();
    }

    info.GetReturnValue().Set(v8::Number::New(textureObject->GetIsolate(), height));
  }

  void Graphics::getTextureLoadedState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> textureObject = info.Holder();
    GraphicsTexture * graphicsTexture = getTextureFromValue(textureObject, "getTextureLoadedState");
    bool loaded = false;

    if (graphicsTexture != nullptr) {
      loaded = graphicsTexture->texture->IsLoaded();
    }

    info.GetReturnValue().Set(v8::Boolean::New(textureObject->GetIsolate(), loaded));
  }

  void Graphics::loadTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no filename given to loadTexture" << std::endl;
      return;
    }

    Graphics * graphics = static_cast<Graphics *>(v8::Local<v8::External>::Cast(info.Holder()->GetInternalField(0))->Value());
    v8::Isolate * isolate = info.This()->GetIsolate();
    std::string filename = ValueToSTDString(info[0]);
    Texture * texture = new Texture(filename);
    GraphicsTexture * graphicsTexture = new GraphicsTexture(graphics, texture);

    v8::Local<v8::ObjectTemplate> textureObjectTemplate = v8::ObjectTemplate::New(isolate);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "width"), &getTextureWidth);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "height"), &getTextureHeight);
    textureObjectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "loaded"), &getTextureLoadedState);

    textureObjectTemplate->SetInternalFieldCount(2);

    v8::Local<v8::Object> textureObject = textureObjectTemplate->NewInstance();
    v8::Local<v8::Number> objectType = v8::Number::New(isolate, ObjectType::TEXTURE);
    v8::Persistent<v8::Object> pTextureObject;

    textureObject->SetInternalField(0, objectType);
    textureObject->SetInternalField(1, v8::External::New(isolate, graphicsTexture));

    pTextureObject.Reset(isolate, textureObject);
    pTextureObject.SetWeak(graphicsTexture, onTextureObjectDestroyed, v8::WeakCallbackType::kParameter);
    pTextureObject.MarkIndependent();

    LoadTextureJob * job = new LoadTextureJob(graphics->display, texture);

    graphics->worker->PushJob(static_cast<Job *>(job));
    info.GetReturnValue().Set(textureObject);
  }

  void Graphics::drawTexture(const v8::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() == 0) {
      std::cerr << "Error: no texture given to drawTexture" << std::endl;
      return;
    }

    GraphicsTexture * graphicsTexture = getTextureFromValue(info[0], "drawTexture");

    if (graphicsTexture != nullptr) {
      int x = info.Length() > 1 ? info[1]->Int32Value() : 0;
      int y = info.Length() > 2 ? info[2]->Int32Value() : 0;
      int width = info.Length() > 3 ? info[3]->Int32Value() : -1;
      int height = info.Length() > 4 ? info[4]->Int32Value() : -1;
      int ox = info.Length() > 5 ? info[5]->Int32Value() : -1;
      int oy = info.Length() > 6 ? info[6]->Int32Value() : -1;

      graphicsTexture->texture->Draw(graphicsTexture->graphics->display, x, y, width, height, ox, oy);
    }
  }

  Graphics::Graphics(
    v8::Isolate * isolate,
    SDLDisplay * display, Worker * worker
  ) : Feeder::Feeder(isolate), display(display), worker(worker) {
    v8::Context::Scope context_scope(context);

    v8::Local<v8::ObjectTemplate> graphicsTemplate = v8::ObjectTemplate::New(isolate);
    graphicsTemplate->SetInternalFieldCount(1);
    graphicsTemplate->Set(v8::String::NewFromUtf8(isolate, "loadTexture"), v8::FunctionTemplate::New(isolate, &loadTexture));
    graphicsTemplate->Set(v8::String::NewFromUtf8(isolate, "drawTexture"), v8::FunctionTemplate::New(isolate, &drawTexture));
    v8::Local<v8::Object> localRoot = graphicsTemplate->NewInstance();
    localRoot->SetInternalField(0, v8::External::New(isolate, this));


    root.Reset(isolate, localRoot);
  }

  GraphicsTexture::GraphicsTexture(Graphics * graphics, Texture * texture) : graphics(graphics), texture(texture) {}
}
