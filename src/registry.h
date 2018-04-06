#ifndef PURR_REGISTRY_H
#define PURR_REGISTRY_H

#include <v8/v8.h>

namespace purr {
  template <class T> class Registry {
    public:
      virtual T * FindAbsolute(std::string) = 0;
      virtual T * FindRelative(std::string, std::string) = 0;
  };
}

#endif
