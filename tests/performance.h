#ifndef LOX_TESTS_PERFORMANCE_H
#define LOX_TESTS_PERFORMANCE_H

#include <string>
#include <iostream>
#include <chrono>

template <typename Func>
inline auto measure(const std::string& test_name, Func&& func) {
  auto start = std::chrono::steady_clock::now();

  std::forward<Func>(func)();

  auto end = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  std::cout << "[" << test_name << "] " << duration << " ms\n";
}

#endif
