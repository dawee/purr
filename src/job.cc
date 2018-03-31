#include "job.h"

namespace purr {
  Job::Job(unsigned type) : type(type) {}
  void Job::Run() {}

  NoopJob::NoopJob() : Job(JobType::NOOP) {}
  void NoopJob::Run() {}

  LoadTextureJob::LoadTextureJob(
    SDLDisplay * display,
    Texture * texture
  ) : Job(JobType::LOAD_TEXTURE), display(display), texture(texture) {}

  void LoadTextureJob::Run() {
    texture->Load(display);
  }

  DestroyTextureJob::DestroyTextureJob(Texture * texture) : Job(JobType::DESTROY_TEXTURE), texture(texture) {}

  void DestroyTextureJob::Run() {
    delete texture;
  }

}
