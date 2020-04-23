#ifndef LOX_TESTS_HELPER_H
#define LOX_TESTS_HELPER_H

#include <ostream>
#include <string>

#include "chunk.h"
#include "compiler.h"
#include "object.h"
#include "scanner.h"
#include "vm.h"

inline std::string compile(std::string source,
                           const std::string& message) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::VM vm{oss};
  auto func = lox::compiler{vm.heap()}.compile(scanner.scan());
  return lox::to_string(func->chunk(), message);
}

template <bool Debug = false>
inline std::string run(std::string source) noexcept {
  std::ostringstream oss;
  lox::VM vm{oss};
  vm.interpret<Debug>(std::move(source));
  return oss.str();
}

#endif
