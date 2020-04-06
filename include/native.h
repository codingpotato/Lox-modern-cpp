#ifndef LOX_NATIVE_H
#define LOX_NATIVE_H

#include <chrono>

#include "hash_table.h"
#include "heap.h"
#include "value.h"

namespace lox {

inline value clock(int, value*) noexcept {
  auto time = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::steady_clock::now().time_since_epoch())
                  .count();
  return static_cast<double>(time) /
         std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::seconds{1})
             .count();
}

inline void register_natives(hash_table& globals, heap& main_heap) noexcept {
  globals.insert(main_heap.make_string("clock"),
                 main_heap.make_object<Native_func>(clock));
}

}  // namespace lox

#endif
