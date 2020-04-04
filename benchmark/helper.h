#ifndef LOX_BENCHMARK_HELPER_H
#define LOX_BENCHMARK_HELPER_H

#include <string>

inline void run(std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  lox::compiler compiler{vm};
  vm.interpret(compiler.compile(scanner.scan()));
}

#endif
