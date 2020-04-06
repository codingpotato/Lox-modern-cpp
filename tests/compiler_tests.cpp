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
  auto func = lox::compiler{vm}.compile(scanner.scan());
  return to_string(func->code, message);
}

TEST_CASE("primary") {
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
0012    3 op_nil
0013    | op_return
)";
  CHECK_EQ(compile("primary", source), expected);
}

TEST_CASE("function call") {
  const std::string source{R"(
fun add(a, b) { return a + b; }
print add(1, 2);
)"};
  const std::string expected = R"(== function call ==
0000    2 op_constant <function: add>
    0000    2 op_get_local 0
    0001    | op_get_local 1
    0002    | op_add
    0003    | op_return
    0004    | op_nil
    0005    | op_return

0001    | op_define_global 0
0002    3 op_get_global 2
0003    | op_constant 1.000000
0004    | op_constant 2.000000
0005    | op_call 2
0006    | op_print
0007    4 op_nil
0008    | op_return
)";
  CHECK_EQ(compile("function call", source), expected);
}
