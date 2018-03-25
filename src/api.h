#ifndef PURR_API_H
#define PURR_API_H

#include <v8/v8.h>
#include "feeder.h"

namespace purr {
  class API : public Feeder {
    public:
      static void LoadTexture(const v8::FunctionCallbackInfo<v8::Value>&);

      API(v8::Isolate*);
  };
}

#endif
