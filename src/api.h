#ifndef PURR_API_H
#define PURR_API_H

#include <v8/v8.h>

#include "display.h"
#include "feeder.h"
#include "worker.h"

namespace purr {
  enum ObjectType {
    TEXTURE
  };

  class APITexture;

  class API : public Feeder {
    private:
      SDLDisplay * display;
      Worker * worker;

      static APITexture * getTextureFromValue(v8::Local<v8::Value>, const char *);
      static void getTextureWidth(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void getTextureHeight(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void getTextureLoadedState(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void loadTexture(const v8::FunctionCallbackInfo<v8::Value>&);
      static void onTextureObjectDestroyed(const v8::WeakCallbackInfo<APITexture> &);
      static void drawTexture(const v8::FunctionCallbackInfo<v8::Value>&);

    public:
      API(v8::Isolate *, SDLDisplay *, Worker *);
  };

  class APITexture {
    private:
      API * api;
      Texture * texture;

      APITexture(API *, Texture *);

    friend class API;
  };
}

#endif
