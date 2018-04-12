#ifndef PURR_FEEDER_H
#define PURR_FEEDER_H

#include <v8/v8.h>

namespace purr {
  class Feeder {
    protected:
      v8::Isolate * isolate;
      v8::Persistent<v8::Value> root;
      v8::Local<v8::Context> context;

    public:
      void FeedObject(const char *, v8::Local<v8::Object>);

      Feeder(v8::Isolate*);
  };
}

#endif
