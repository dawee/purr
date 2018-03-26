#ifndef PURR_CONSOLE_H
#define PURR_CONSOLE_H

#include <v8/v8.h>
#include "feeder.h"

namespace purr {
  class Console : public Feeder {
    public:
      static void Log(const v8::FunctionCallbackInfo<v8::Value>&);

      Console(v8::Isolate *);
  };
}

#endif
