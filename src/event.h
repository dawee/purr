#ifndef PURR_EVENT_H
#define PURR_EVENT_H

#include <SDL2/SDL.h>
#include <v8/v8.h>

namespace purr {
  class Event {
    protected:
      Event();

    public:
      static Event * FromSDLEvent(SDL_Event& event);
      static void Destroy(Event *);

      virtual v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>);
  };

  class KeyboardEvent : public Event {
    protected:
      const char * key;

    public:
      KeyboardEvent(SDL_Event& event);
  };

  class KeydownEvent : public KeyboardEvent {
    public:
      v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>);

      KeydownEvent(SDL_Event& event);
  };

  class KeyupEvent : public KeyboardEvent {
    public:
      v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>);

      KeyupEvent(SDL_Event& event);
  };

  class ControllerAxisEvent : public Event {
    private:
      int axis;
      double value;

    public:
      v8::Local<v8::Object> ToJS(v8::Isolate *, v8::Local<v8::Context>);

      ControllerAxisEvent(SDL_Event& event);
  };
}

#endif
