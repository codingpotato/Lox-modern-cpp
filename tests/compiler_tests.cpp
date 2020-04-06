#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "compiler.h"
#include "vm.h"

inline std::string compile(const std::string& message,
                           std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::Vm vm{oss};
  auto func = lox::compiler{vm}.compile(scanner.scan());
  return to_string(func->code(), message);
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
0012    3 op_nil
0013    | op_return
)";
  CHECK_EQ(compile("primary", source), expected);
}

TEST_CASE("compile function call") {
  const std::string source{R"(
fun add(a, b) { return a + b; }
print add(1, 2);
)"};
  const std::string expected = R"(== function call ==
0000    2 op_constant <function: add>
    0000    2 op_get_local 1
    0001    | op_get_local 2
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

TEST_CASE("compile fib") {
  const std::string source{R"(
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}
print fib(30);
)"};
  const std::string expected = R"(== fib ==
0000    5 op_constant <function: fib>
    0000    3 op_get_local 1
    0001    | op_constant 2.000000
    0002    | op_less
    0003    | op_jump_if_false 4
    0004    | op_pop
    0005    | op_get_local 1
    0006    | op_return
    0007    | op_jump 1
    0008    | op_pop
    0009    4 op_get_global 1
    0010    | op_get_local 1
    0011    | op_constant 2.000000
    0012    | op_subtract
    0013    | op_call 1
    0014    | op_get_global 3
    0015    | op_get_local 1
    0016    | op_constant 1.000000
    0017    | op_subtract
    0018    | op_call 1
    0019    | op_add
    0020    | op_return
    0021    5 op_nil
    0022    | op_return

0001    | op_define_global 0
0002    6 op_get_global 2
0003    | op_constant 30.000000
0004    | op_call 1
0005    | op_print
0006    7 op_nil
0007    | op_return
)";
  CHECK_EQ(compile("fib", source), expected);
}