#ifndef PURR_JOB_H
#define PURR_JOB_H

#include "display.h"
#include "texture.h"

namespace purr {
  enum JobType {
    NOOP,
    LOAD_TEXTURE,
    DESTROY_TEXTURE
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
      SDLDisplay * display;
      Texture * texture;

    public:
      LoadTextureJob(SDLDisplay * display, Texture *);
      void Run();
  };

  class DestroyTextureJob : public Job {
    private:
      Texture * texture;

    public:
      DestroyTextureJob(Texture *);
      void Run();
  };
}

#endif
