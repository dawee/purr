#include "event.h"

static const char * STR_TYPE = "type";
static const char * STR_KEYDOWN = "keydown";
static const char * STR_KEYUP = "keyup";
static const char * STR_KEY = "key";

namespace purr {
  Event * Event::FromSDLEvent(SDL_Event& sdlEvent) {
    switch (sdlEvent.type) {
      case SDL_KEYDOWN:
        return new KeydownEvent(sdlEvent);
      case SDL_KEYUP:
        return new KeyupEvent(sdlEvent);
      default:
        return nullptr;
    };
  }

  void Event::Destroy(Event * event) {
    switch (event->type) {
      case EventType::KEYDOWN: {
        KeydownEvent * keydownEvent = static_cast<KeydownEvent *>(event);
        delete keydownEvent;
        break;
      }
      case EventType::KEYUP: {
        KeyupEvent * keyupEvent = static_cast<KeyupEvent *>(event);
        delete keyupEvent;
        break;
      }
      default:
        break;
    };
  }

  /*
   * KeyboardEvent
   */

  KeyboardEvent::KeyboardEvent(SDL_Event& sdlEvent) {
    key = SDL_GetKeyName(sdlEvent.key.keysym.sym);
  }

  /*
   * KeydownEvent
   */

  v8::Local<v8::Object> KeydownEvent::ToJS(v8::Isolate * isolate, v8::Local<v8::Context> context) {
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Object> jsEvent = v8::Object::New(isolate);

    jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_TYPE), v8::String::NewFromUtf8(isolate, STR_KEYDOWN));
    jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_KEY), v8::String::NewFromUtf8(isolate, key));

    return jsEvent;
  }

  KeydownEvent::KeydownEvent(SDL_Event& sdlEvent) : KeyboardEvent(sdlEvent) {
    type = EventType::KEYDOWN;
  }

  /*
   * KeyupEvent
   */

   v8::Local<v8::Object> KeyupEvent::ToJS(v8::Isolate * isolate, v8::Local<v8::Context> context) {
     v8::Context::Scope context_scope(context);
     v8::Local<v8::Object> jsEvent = v8::Object::New(isolate);

     jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_TYPE), v8::String::NewFromUtf8(isolate, STR_KEYUP));
     jsEvent->Set(v8::String::NewFromUtf8(isolate, STR_KEY), v8::String::NewFromUtf8(isolate, key));

     return jsEvent;
   }

   KeyupEvent::KeyupEvent(SDL_Event& sdlEvent) : KeyboardEvent(sdlEvent) {
     type = EventType::KEYUP;
   }
}
