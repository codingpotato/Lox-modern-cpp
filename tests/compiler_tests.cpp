#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "helper.h"

TEST_CASE("compiler: primary") {
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
0012    | OP_Constant str
0014    | OP_Pop
0015    3 OP_Nil
0016    | OP_Return
)";
  CHECK_EQ(compile(source, "primary"), expected);
}

TEST_CASE("compiler: if") {
  const std::string source{R"(
if (1 > 0) print 1;
else print 0;
)"};
  const std::string expected = R"(== if ==
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
  CHECK_EQ(compile(source, "if"), expected);
}

TEST_CASE("compiler: while") {
  const std::string source{R"(
while (1 > 0) print 1;
)"};
  const std::string expected = R"(== while ==
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
  CHECK_EQ(compile(source, "while"), expected);
}

TEST_CASE("compiler: function call") {
  const std::string source{R"(
fun add(a, b) { return a + b; }
print add(1, 2);
)"};
  const std::string expected = R"(== function call ==
0000    2 OP_Closure <func: add>
    0000    2 OP_Get_local 1
    0002    | OP_Get_local 2
    0004    | OP_Add
    0005    | OP_Return
    0006    | OP_Nil
    0007    | OP_Return
        upvalues: 
0002    | OP_Define_global add
0004    3 OP_Get_global add
0006    | OP_Constant 1.000000
0008    | OP_Constant 2.000000
0010    | OP_Call 2
0012    | OP_Print
0013    4 OP_Nil
0014    | OP_Return
)";
  CHECK_EQ(compile(source, "function call"), expected);
}

TEST_CASE("compiler: fib") {
  const std::string source{R"(
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}
print fib(8);
)"};
  const std::string expected = R"(== fib ==
0000    5 OP_Closure <func: fib>
    0000    3 OP_Get_local 1
    0002    | OP_Constant 2.000000
    0004    | OP_Less
    0005    | OP_Jump_if_false 7 -> 15
    0008    | OP_Pop
    0009    | OP_Get_local 1
    0011    | OP_Return
    0012    | OP_Jump 1 -> 16
    0015    | OP_Pop
    0016    4 OP_Get_global fib
    0018    | OP_Get_local 1
    0020    | OP_Constant 2.000000
    0022    | OP_Subtract
    0023    | OP_Call 1
    0025    | OP_Get_global fib
    0027    | OP_Get_local 1
    0029    | OP_Constant 1.000000
    0031    | OP_Subtract
    0032    | OP_Call 1
    0034    | OP_Add
    0035    | OP_Return
    0036    5 OP_Nil
    0037    | OP_Return
        upvalues: 
0002    | OP_Define_global fib
0004    6 OP_Get_global fib
0006    | OP_Constant 8.000000
0008    | OP_Call 1
0010    | OP_Print
0011    7 OP_Nil
0012    | OP_Return
)";
  CHECK_EQ(compile(source, "fib"), expected);
}

TEST_CASE("compiler: and truth") {
  const std::string source{R"(
print false and "bad";
print nil and "bad";
print true and "ok";
print 0 and "ok";
)"};
  const std::string expected = R"(== and truth ==
0000    2 OP_False
0001    | OP_Jump_if_false 3 -> 7
0004    | OP_Pop
0005    | OP_Constant bad
0007    | OP_Print
0008    3 OP_Nil
0009    | OP_Jump_if_false 3 -> 15
0012    | OP_Pop
0013    | OP_Constant bad
0015    | OP_Print
0016    4 OP_True
0017    | OP_Jump_if_false 3 -> 23
0020    | OP_Pop
0021    | OP_Constant ok
0023    | OP_Print
0024    5 OP_Constant 0.000000
0026    | OP_Jump_if_false 3 -> 32
0029    | OP_Pop
0030    | OP_Constant ok
0032    | OP_Print
0033    6 OP_Nil
0034    | OP_Return
)";
  CHECK_EQ(compile(source, "and truth"), expected);
}
