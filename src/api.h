#ifndef PURR_API_H
#define PURR_API_H

#include <v8/v8.h>
#include "feeder.h"

namespace purr {
  enum ObjectType {
    TEXTURE
  };

  class API : public Feeder {
    public:
      API(v8::Isolate*);
  };
}

#endif
