#ifndef LOX_TESTS_HELPER_H
#define LOX_TESTS_HELPER_H

#include <ostream>
#include <string>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

inline std::string run(std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  lox::compiler compiler{vm};
  vm.interpret(compiler.compile(scanner.scan()));
  return oss.str();
}

#endif
