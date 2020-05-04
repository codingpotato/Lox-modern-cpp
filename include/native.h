#ifndef LOX_NATIVE_H
#define LOX_NATIVE_H

#include <chrono>

#include "hash_table.h"
#include "heap.h"
#include "value.h"

namespace lox {

inline Value clock(int, Value*) noexcept {
  constexpr auto one_second =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::seconds{1})
          .count();
  const auto time = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now().time_since_epoch())
                        .count();
  return static_cast<double>(time) / one_second;
}

inline void register_natives(Hash_table& globals, Heap<>& heap) noexcept {
  const auto name = heap.make_string("clock");
  globals.insert(name, Value{});
  globals.set(name, heap.make_object<Native_func>(clock));
}

}  // namespace lox

#endif
