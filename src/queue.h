#ifndef PURR_QUEUE_H
#define PURR_QUEUE_H

#include <stack>

namespace purr {
  template <class T> class Queue {
    private:
      SDL_sem * sem;
      std::stack<T *> stack;

    public:
      Queue();
      void Push(T *);
      T * Pull();
  };  
}

#endif
