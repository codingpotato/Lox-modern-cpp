#ifndef LOX_TESTS_HELPER_H
#define LOX_TESTS_HELPER_H

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "compiler.h"
#include "heap.h"
#include "object.h"
#include "scanner.h"
#include "vm.h"

inline std::string expected_from(const std::string& source) noexcept {
  std::string expected;
  const std::string key = "// expect: ";
  auto it = source.cbegin();
  while (it != source.cend()) {
    it = std::search(it, source.cend(), key.cbegin(), key.cend());
    if (it != source.end()) {
      it += key.size();
      const auto last = std::find(it, source.cend(), '\n');
      expected += {it, last};
      expected += '\n';
      it = last;
    }
  }
  return expected;
}

inline std::pair<std::string, std::string> load(const std::string& filename) {
  std::ifstream ifs{filename};
  auto source = std::string{std::istreambuf_iterator<char>{ifs},
                            std::istreambuf_iterator<char>{}};
  if (ifs.good()) {
    auto expected = expected_from(source);
    return {std::move(source), std::move(expected)};
  }
  throw std::runtime_error{"Read " + filename + " failed."};
}

inline std::string compile(std::string source,
                           const std::string& message) noexcept {
  lox::Scanner scanner{std::move(source)};
  lox::Heap heap;
  lox::Compiler compiler{heap};
  auto func = compiler.compile(scanner.scan());
  return lox::to_string(func->get_chunk(), message);
}

template <bool Debug = false>
inline std::string run(std::string source) noexcept {
  std::ostringstream oss;
  lox::VM vm{oss};
  vm.interpret<Debug>(std::move(source));
  return oss.str();
}

#endif
