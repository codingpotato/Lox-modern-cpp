#include "scanner.h"

namespace lox {

Compile_error Scanner::make_compiler_error(const std::string& message) const
    noexcept {
  std::string string = {start, current};
  return Compile_error{"[line " + std::to_string(line) + "] " +
                       (!string.empty() ? "Error at '" + string + "': " : "") +
                       message};
}

}  // namespace lox
