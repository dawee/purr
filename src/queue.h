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

      T * pop() {
        T * item = nullptr;

        if (SDL_LockMutex(mutex) == 0) {
          if (q.size() > 0) {
            item = q.front();
            q.pop();
          }

          SDL_UnlockMutex(mutex);
        }

        return item;
      }

      void push(T * item) {
        if (SDL_LockMutex(mutex) == 0) {
          q.push(item);
          SDL_UnlockMutex(mutex);
        }
      }

    public:
      Queue() {
        sem = SDL_CreateSemaphore(0);
        mutex = SDL_CreateMutex();
      }

      void Push(T * item) {
        push(item);
        SDL_SemPost(sem);
      }

      T * Pull() {
        SDL_SemWait(sem);
        return pop();
      }

      T * PullWithoutWaiting() {
        SDL_SemTryWait(sem);
        return pop();
      }
  };
}

#endif
