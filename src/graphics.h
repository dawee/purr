#ifndef PURR_Graphics_H
#define PURR_Graphics_H

#include <v8/v8.h>

#include "display.h"
#include "feeder.h"
#include "worker.h"

namespace purr {
  enum ObjectType {
    TEXTURE
  };

  class GraphicsTexture;

  class Graphics : public Feeder {
    private:
      SDLDisplay * display;
      Worker * worker;

      static GraphicsTexture * getTextureFromValue(v8::Local<v8::Value>, const char *);
      static void getTextureWidth(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void getTextureHeight(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void getTextureLoadedState(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
      static void loadTexture(const v8::FunctionCallbackInfo<v8::Value>&);
      static void onTextureObjectDestroyed(const v8::WeakCallbackInfo<GraphicsTexture> &);
      static void drawTexture(const v8::FunctionCallbackInfo<v8::Value>&);

    public:
      Graphics(v8::Isolate *, SDLDisplay *, Worker *);
  };

  class GraphicsTexture {
    private:
      Graphics * graphics;
      Texture * texture;

      GraphicsTexture(Graphics *, Texture *);

    friend class Graphics;
  };
}

#endif
