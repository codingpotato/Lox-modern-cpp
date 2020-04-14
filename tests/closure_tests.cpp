#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("compile closure") {
  const std::string source{R"(
fun makeClosure() {
  var local = "local";
  fun closure() {
    print local;
  }
  return closure;
}
var closure = makeClosure();
closure();
)"};
  const std::string expected = R"(== primary ==
0000    8 OP_Closure <function: makeClosure>
    0000    3 OP_Constant "local"
    0002    6 OP_Closure <function: closure>
    0000    5 OP_Get_upvalue 0
    0002    | OP_Print
    0003    6 OP_Nil
    0004    | OP_Return
        upvalues:  local 1,
    0004    | OP_Nil
    0005    | OP_Nil
    0006    7 OP_Get_local 2
    0008    | OP_Return
    0009    8 OP_Nil
    0010    | OP_Return
        upvalues: 
0002    | OP_Define_global makeClosure
0004    9 OP_Get_global makeClosure
0006    | OP_Call 0
0008    | OP_Define_global closure
0010   10 OP_Get_global closure
0012    | OP_Call 0
0014    | OP_Pop
0015   11 OP_Nil
0016    | OP_Return
)";
  CHECK_EQ(compile(source, "primary"), expected);
}

TEST_CASE("closure") {
  std::string source{R"(
fun makeClosure() {
  var local = "local";
  fun closure() {
    print local;
  }
  return closure;
}
var closure = makeClosure();
closure();
)"};
  std::string expected{R"("local"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("compile closure") {
  const std::string source{R"(
var f;
var g;
{
  var local = "local";
  fun f_() {
    print local;
    local = "after f";
    print local;
  }
  f = f_;
  fun g_() {
    print local;
    local = "after g";
    print local;
  }
  g = g_;
}
f();
g();
)"};
  const std::string expected = R"(== primary ==
0000    8 OP_Closure <function: makeClosure>
    0000    3 OP_Constant "local"
    0002    6 OP_Closure <function: closure>
    0000    5 OP_Get_upvalue 0
    0002    | OP_Print
    0003    6 OP_Nil
    0004    | OP_Return
        upvalues:  local 1,
    0004    | OP_Nil
    0005    | OP_Nil
    0006    7 OP_Get_local 2
    0008    | OP_Return
    0009    8 OP_Nil
    0010    | OP_Return
        upvalues: 
0002    | OP_Define_global makeClosure
0004    9 OP_Get_global makeClosure
0006    | OP_Call 0
0008    | OP_Define_global closure
0010   10 OP_Get_global closure
0012    | OP_Call 0
0014    | OP_Pop
0015   11 OP_Nil
0016    | OP_Return
)";
  CHECK_EQ(compile(source, "primary"), expected);
}

TEST_CASE("assign to closure") {
  std::string source{R"(
var f;
var g;
{
  var local = "local";
  fun f_() {
    print local;
    local = "after f";
    print local;
  }
  f = f_;
  fun g_() {
    print local;
    local = "after g";
    print local;
  }
  g = g_;
}
print f;
f();
g();
)"};
  std::string expected{R"("local"
"after f"
"after f"
"after g"
)"};
  CHECK_EQ(run<true>(source), expected);
}
