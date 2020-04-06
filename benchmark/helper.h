#ifndef LOX_BENCHMARK_HELPER_H
#define LOX_BENCHMARK_HELPER_H

#include <ostream>
#include <string>

#include "compiler.h"
#include "scanner.h"
#include "vm.h"

inline void run(std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::Vm vm{oss};
  lox::compiler compiler{vm};
  vm.interpret(compiler.compile(scanner.scan()));
}

#endif
