#ifndef LOX_GC_H
#define LOX_GC_H

#include <vector>

#include "contract.h"
#include "object.h"
#include "value.h"

namespace lox {

class Memory_tracker {
 public:
  Memory_tracker() noexcept { current_tracker = this; }

  virtual ~Memory_tracker() noexcept { current_tracker = nullptr; }

  Memory_tracker(const Memory_tracker&) = delete;
  Memory_tracker(Memory_tracker&&) = delete;
  Memory_tracker& operator=(const Memory_tracker&) = delete;
  Memory_tracker& operator=(Memory_tracker&&) = delete;

  static Memory_tracker* current() noexcept { return current_tracker; }

  void allocate(size_t size) noexcept {
    bytes_allocated += size;
    if (bytes_allocated > next_gc) {
      collect_garbage();
      next_gc = bytes_allocated * 2;
    }
  }
  void free(size_t size) noexcept { bytes_allocated -= size; }

 private:
  virtual void collect_garbage() noexcept = 0;

  static constexpr size_t initial_gc = 1024 * 1024;
  static constexpr size_t heap_grow_factor = 2;

  inline static Memory_tracker* current_tracker = nullptr;

  size_t bytes_allocated = 0;
  size_t next_gc = initial_gc;
};

template <typename Heap, typename Hash_table, typename Value_stack,
          typename Call_frame_stack, typename Compiler>
class GC : Memory_tracker {
 public:
  GC(Heap& heap, const Hash_table& globals, const Value_stack& stack,
     const Call_frame_stack& call_frames, const Compiler& compiler)
  noexcept
      : heap{&heap},
        globals{&globals},
        stack{&stack},
        call_frames{&call_frames},
        compiler{&compiler} {}

  void collect_garbage() noexcept override {
    mark_roots();
    trace_references();
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
      mark_object(func->name);

      for (auto constant : func->get_chunk().get_constants()) {
        mark_value(constant);
      }
    } else if (object->is<Upvalue>()) {
      mark_value(object->as<Upvalue>()->closed);
    }
  }

  Heap* heap;
  const Hash_table* globals;
  const Value_stack* stack;
  const Call_frame_stack* call_frames;
  const Compiler* compiler;
  std::vector<Object*> gray_objects;
};

}  // namespace lox

#endif
