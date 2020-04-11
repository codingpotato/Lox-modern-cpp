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
  return to_string(func->chunk(), message);
}

TEST_CASE("compile primary") {
  const std::string source{R"(1; 2; nil;
  true; false; "str";
)"};
  const std::string expected = R"(== primary ==
0000    1 OP_Constant 1.000000
0002    | OP_Pop
0003    | OP_Constant 2.000000
0005    | OP_Pop
0006    | OP_Nil
0007    | OP_Pop
0008    2 OP_True
0009    | OP_Pop
0010    | OP_False
0011    | OP_Pop
0012    | OP_Constant "str"
0014    | OP_Pop
0015    3 OP_Nil
0016    | OP_Return
)";
  CHECK_EQ(compile("primary", source), expected);
}

TEST_CASE("compile if statement") {
  const std::string source{R"(
if (1 > 0) print 1;
else print 0;
)"};
  const std::string expected = R"(== primary ==
0000    2 OP_Constant 1.000000
0002    | OP_Constant 0.000000
0004    | OP_Greater
0005    | OP_Jump_if_false 7
0008    | OP_Pop
0009    | OP_Constant 1.000000
0011    | OP_Print
0012    | OP_Jump 4
0015    | OP_Pop
0016    3 OP_Constant 0.000000
0018    | OP_Print
0019    4 OP_Nil
0020    | OP_Return
)";
  CHECK_EQ(compile("primary", source), expected);
}

TEST_CASE("compile while statement") {
  const std::string source{R"(
while (1 > 0) print 1;
)"};
  const std::string expected = R"(== primary ==
0000    2 OP_Constant 1.000000
0002    | OP_Constant 0.000000
0004    | OP_Greater
0005    | OP_Jump_if_false 7
0008    | OP_Pop
0009    | OP_Constant 1.000000
0011    | OP_Print
0012    | OP_Loop 15
0015    | OP_Pop
0016    3 OP_Nil
0017    | OP_Return
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
