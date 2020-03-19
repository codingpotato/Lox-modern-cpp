#ifndef LOX_TESTS_PERFORMANCE_H
#define LOX_TESTS_PERFORMANCE_H

#include <chrono>
#include <iostream>
#include <string>

namespace lox {

template <typename Func>
inline void measure(const std::string& name, int count, Func&& func) noexcept {
  auto start = std::chrono::steady_clock::now();
  for (auto i = 0; i < count; ++i) {
    std::forward<Func>(func)();
  }
  auto end = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "[" << name << "] " << duration.count() << "ms\n";
}

}  // namespace lox

#endif
