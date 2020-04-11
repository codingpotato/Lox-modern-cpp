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
0005    | OP_Jump_if_false 7 -> 15
0008    | OP_Pop
0009    | OP_Constant 1.000000
0011    | OP_Print
0012    | OP_Jump 4 -> 19
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
0005    | OP_Jump_if_false 7 -> 15
0008    | OP_Pop
0009    | OP_Constant 1.000000
0011    | OP_Print
0012    | OP_Loop 15 -> 0
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
0000    2 OP_Closure <function: add>
    0000    2 OP_Get_local 1
    0002    | OP_Get_local 2
    0004    | OP_Add
    0005    | OP_Return
    0006    | OP_Nil
    0007    | OP_Return

0002    | OP_Define_global 0
0004    3 OP_Get_global 2
0006    | OP_Constant 1.000000
0008    | OP_Constant 2.000000
0010    | OP_Call 2
0012    | OP_Print
0013    4 OP_Nil
0014    | OP_Return
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
0000    5 OP_Closure <function: fib>
    0000    3 OP_Get_local 1
    0002    | OP_Constant 2.000000
    0004    | OP_Less
    0005    | OP_Jump_if_false 7 -> 15
    0008    | OP_Pop
    0009    | OP_Get_local 1
    0011    | OP_Return
    0012    | OP_Jump 1 -> 16
    0015    | OP_Pop
    0016    4 OP_Get_global 1
    0018    | OP_Get_local 1
    0020    | OP_Constant 2.000000
    0022    | OP_Subtract
    0023    | OP_Call 1
    0025    | OP_Get_global 3
    0027    | OP_Get_local 1
    0029    | OP_Constant 1.000000
    0031    | OP_Subtract
    0032    | OP_Call 1
    0034    | OP_Add
    0035    | OP_Return
    0036    5 OP_Nil
    0037    | OP_Return

0002    | OP_Define_global 0
0004    6 OP_Get_global 2
0006    | OP_Constant 30.000000
0008    | OP_Call 1
0010    | OP_Print
0011    7 OP_Nil
0012    | OP_Return
)";
  CHECK_EQ(compile("fib", source), expected);
}
