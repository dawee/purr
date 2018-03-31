#ifndef PURR_WORKER_H
#define PURR_WORKER_H

#include "job.h"

namespace purr {
  class Worker {
    public:
      virtual void PushJob(Job *) = 0;
  };
}

#endif
