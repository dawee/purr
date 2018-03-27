#ifndef PURR_JOB_H
#define PURR_JOB_H

#include "texture.h"

namespace purr {
  enum JobType {
    NOOP,
    LOAD_TEXTURE
  };

  class Job {
    private:
      unsigned type;

    protected:
      Job(unsigned);

    public:
      unsigned Type();
      virtual void Run();
  };

  class NoopJob : public Job {
    public:
      NoopJob();
      void Run();
  };

  class LoadTextureJob : public Job {
    private:
      Texture * texture;

    public:
      LoadTextureJob(Texture *);
      void Run();
  };
}

#endif
