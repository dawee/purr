#ifndef PURR_EVENT_H
#define PURR_EVENT_H

#include <SDL2/SDL.h>
#include <v8/v8.h>

namespace purr {
  enum EventType {
    KEYDOWN
  };

  class Event {
    protected:
      EventType type;

    public:
      static Event * FromSDLEvent(SDL_Event& event);
      static void Destroy(Event *);

      virtual v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>) = 0;
  };

  class KeyDownEvent : public Event {
    private:
      const char * key;

    public:
      v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>);

      KeyDownEvent(SDL_Event& event);
  };
}

#endif
