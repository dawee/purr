#ifndef PURR_API_H
#define PURR_API_H

#include <v8/v8.h>
#include "feeder.h"

namespace purr {
  enum ObjectType {
    TEXTURE
  };

  class API : public Feeder {
    public:
      static void DrawTexture(const v8::FunctionCallbackInfo<v8::Value>&);
      static void LoadTexture(const v8::FunctionCallbackInfo<v8::Value>&);

      API(v8::Isolate*);
  };
}

#endif
