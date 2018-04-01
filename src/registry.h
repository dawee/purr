#ifndef PURR_REGISTER_H
#define PURR_REGISTER_H

namespace purr {
  template <class T> class Registry {
    public:
      virtual T * Save(std::string) = 0;
  };
}

#endif
