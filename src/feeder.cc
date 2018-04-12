#include "feeder.h"

namespace purr {
  Feeder::Feeder(v8::Isolate * isolate) : isolate(isolate) {
    context = v8::Context::New(isolate);
  }

  void Feeder::FeedObject(const char * key, v8::Local<v8::Object> object) {
    v8::Local<v8::Value> localRoot = v8::Local<v8::Value>::New(isolate, root);
    object->Set(v8::String::NewFromUtf8(isolate, key), localRoot);
  }
}
