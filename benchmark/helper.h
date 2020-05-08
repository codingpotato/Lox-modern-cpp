#ifndef LOX_BENCHMARK_HELPER_H
#define LOX_BENCHMARK_HELPER_H

#include <fstream>
#include <string>

#include "vm.h"

inline std::string load_source(std::string file_path) noexcept {
  std::ifstream ifs(std::move(file_path));
  return std::string{std::istreambuf_iterator<char>{ifs},
                     std::istreambuf_iterator<char>{}};
}

inline void run(std::string source) noexcept {
  std::ostringstream oss;
  lox::VM vm{oss};
  vm.interpret(std::move(source));
}

#endif
