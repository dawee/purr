#ifndef PURR_REGISTER_H
#define PURR_REGISTER_H

namespace purr {
  template <class T> class Registry {
    public:
      virtual T * GetFromHolder(v8::Local<v8::Object>) = 0;
      virtual T * Save(std::string) = 0;
  };
}

#endif
