#ifndef LOX_BENCHMARK_HELPER_H
#define LOX_BENCHMARK_HELPER_H

#include <ostream>
#include <string>

#include "vm.h"

inline void run(std::string source) noexcept {
  std::ostringstream oss;
  lox::VM vm{oss};
  vm.interpret(std::move(source));
}

#endif
