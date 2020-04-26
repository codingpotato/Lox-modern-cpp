#ifndef LOX_GC_H
#define LOX_GC_H

#include <vector>

#include "compiler.h"
#include "hash_table.h"
#include "heap.h"
#include "stack.h"
#include "value.h"

namespace lox {

template <typename Value_stack, typename Call_frame_stack>
struct GC : Heap<>::Delegate {
  GC(const Value_stack& stack, Heap<>& heap, const Hash_table& globals,
     const Call_frame_stack& call_frames)
  noexcept
      : stack{&stack},
        heap{&heap},
        globals{&globals},
        call_frames{&call_frames} {
    heap.set_delegate(*this);
  }

  void set_compiler(Compiler& c) noexcept { compiler = &c; }

  void collect_garbage() noexcept override {
    mark_roots();
    trace_references();
    heap->erase_unmarked_strings();
    heap->sweep();
  }

 private:
  void mark_roots() noexcept {
    for (size_t i = 0; i < stack->size(); ++i) {
      mark_value((*stack)[i]);
    }
    mark_globals();
    for (size_t i = 0; i < call_frames->size(); ++i) {
      mark_object((*call_frames)[i].closure);
    }
    const auto& open_upvalues = heap->get_open_upvalues();
    for (auto it = open_upvalues.begin(); it != open_upvalues.end(); ++it) {
      mark_object(it);
    }
    if (compiler) {
      compiler->for_each_func([&](Function* func) { mark_object(func); });
    }
  }

  void mark_value(Value value) noexcept {
    if (value.is_object()) {
      mark_object(value.as_object());
    }
  }

  void mark_object(Object* object) noexcept {
    if (object && !object->is_marked) {
      object->is_marked = true;
      gray_objects.push_back(object);
    }
  }

  void mark_globals() noexcept {
    globals->for_each([&](String* key, Value value) {
      mark_object(key);
      mark_value(value);
    });
  }

  void trace_references() noexcept {
    while (!gray_objects.empty()) {
      auto object = gray_objects.back();
      gray_objects.pop_back();
      blacken_object(object);
    }
  }

  void blacken_object(Object* object) noexcept {
    if (object->is<Closure>()) {
      auto closure = object->as<Closure>();
      mark_object(closure->get_func());
      for (auto upvalue : closure->get_upvalues()) {
        mark_object(upvalue);
      }
    } else if (object->is<Function>()) {
      auto func = object->as<Function>();
      mark_object(func->get_name());
      for (auto value : func->get_chunk().constants()) {
        mark_value(value);
      }
    } else if (object->is<Upvalue>()) {
      mark_value(object->as<Upvalue>()->closed);
    }
  }

  const Value_stack* stack;
  Heap<>* heap;
  const Hash_table* globals;
  const Call_frame_stack* call_frames;
  const Compiler* compiler = nullptr;
  std::vector<Object*> gray_objects;
};

}  // namespace lox

#endif
