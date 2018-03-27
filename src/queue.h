#ifndef PURR_QUEUE_H
#define PURR_QUEUE_H

#include <queue>
#include <SDL2/SDL.h>

namespace purr {
  template <class T> class Queue {
    private:
      SDL_sem * sem;
      SDL_mutex * mutex;
      std::queue<T *> q;

      T * pop();
      void push(T *);

    public:
      Queue();
      void Push(T *);
      T * Pull();
  };
}

#endif
