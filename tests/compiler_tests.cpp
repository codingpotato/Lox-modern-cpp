#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "compiler.h"
#include "virtual_machine.h"

inline std::string compile(const std::string& message,
                           std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  lox::compiler{vm}.compile(scanner.scan());
  return vm.main().repr(message);
}

TEST_CASE("compile primary") {
  const std::string source{R"(1; 2; nil;
  true; false; "str";
)"};
  const std::string expected = R"(== primary ==
0000    1 op_constant 1.000000
0001    | op_pop
0002    | op_constant 2.000000
0003    | op_pop
0004    | op_nil
0005    | op_pop
0006    2 op_true
0007    | op_pop
0008    | op_false
0009    | op_pop
0010    | op_constant "str"
0011    | op_pop
)";
  CHECK_EQ(compile("primary", source), expected);
}
