#include "event.h"

static const char * STR_TYPE = "type";
static const char * STR_KEYDOWN = "keydown";
static const char * STR_KEY = "key";

namespace purr {
  Event * Event::FromSDLEvent(SDL_Event& sdlEvent) {
    switch (sdlEvent.type) {
      case SDL_KEYDOWN:
        return new KeyDownEvent(sdlEvent);
      default:
        return nullptr;
    };
  }

  void Event::Destroy(Event * event) {
    switch (event->type) {
      case EventType::KEYDOWN: {
        KeyDownEvent * keyDownEvent = static_cast<KeyDownEvent *>(event);
        delete keyDownEvent;
        break;
      }
      default:
        break;
    };
  }

  v8::Local<v8::Object> KeyDownEvent::ToJS(v8::Isolate * isolate, v8::Local<v8::Context> context) {
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Object> jsEvent = v8::Object::New(isolate);

    jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_TYPE), v8::String::NewFromUtf8(isolate, STR_KEYDOWN));
    jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_KEY), v8::String::NewFromUtf8(isolate, key));

    return jsEvent;
  }

  KeyDownEvent::KeyDownEvent(SDL_Event& sdlEvent) {
    type = EventType::KEYDOWN;
    key = SDL_GetKeyName(sdlEvent.key.keysym.sym);
  }
}
