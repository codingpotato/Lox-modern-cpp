#ifndef LOX_GC_H
#define LOX_GC_H

#include "heap.h"
#include "stack.h"
#include "value.h"

namespace lox {

template <size_t Max_stack_size>
struct GC : Heap<>::Delegate {
  GC(const Stack<Value, Max_stack_size>& stack, Heap<>& heap)
  noexcept : stack{&stack}, heap{&heap} {
    heap.set_delegate(*this);
  }

  void collect_garbage() const noexcept override { mark_roots(); }

 private:
  void mark_roots() const noexcept {
    for (size_t i = 0; i < stack->size(); ++i) {
      mark_value((*stack)[i]);
    }
  }

  void mark_value(Value value) const noexcept {
    if (value.is_object()) {
      mark_object(value.as_object());
    }
  }

  void mark_object(Object* object) const noexcept {
    if (object) {
      object->is_marked = true;
    }
  }

  const Stack<Value, Max_stack_size>* stack;
  const Heap<>* heap;
};

}  // namespace lox

#endif
