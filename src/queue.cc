#include "queue.h"

namespace purr {
  template <class T> Queue<T>::Queue() {
    sem = SDL_CreateSemaphore(0);
    mutex = SDL_CreateMutex();
  }

  template <class T> T * Queue<T>::pop() {
    T * item = nullptr;

    if (SDL_LockMutex(mutex) == 0) {
      item = q.pop();
      SDL_UnlockMutex(mutex);
    }

    return item;
  }

  template <class T> void Queue<T>::push(T * item) {
    if (SDL_LockMutex(mutex) == 0) {
      q.push(item);
      SDL_UnlockMutex(mutex);
    }
  }

  template <class T> void Queue<T>::Push(T * item) {
    push(item);
    SDL_SemPost(sem);
  }

  template <class T> T * Queue<T>::Pull() {
    SDL_SemWait(sem);
    return pop();
  }
}
