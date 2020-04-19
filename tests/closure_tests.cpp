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
0000    2 OP_Nil
0001    | OP_Define_global f
0003    3 OP_Nil
0004    | OP_Define_global g
0006    5 OP_Constant "local"
0008   10 OP_Closure <function: f_>
    0000    7 OP_Get_upvalue 0
    0002    | OP_Print
    0003    8 OP_Constant "after f"
    0005    | OP_Set_upvalue 0
    0007    | OP_Pop
    0008    9 OP_Get_upvalue 0
    0010    | OP_Print
    0011   10 OP_Nil
    0012    | OP_Return
        upvalues:  local 1,
0011    | OP_Nil
0012   11 OP_Get_local 2
0014    | OP_Set_global f
0016    | OP_Pop
0017   16 OP_Closure <function: g_>
    0000   13 OP_Get_upvalue 0
    0002    | OP_Print
    0003   14 OP_Constant "after g"
    0005    | OP_Set_upvalue 0
    0007    | OP_Pop
    0008   15 OP_Get_upvalue 0
    0010    | OP_Print
    0011   16 OP_Nil
    0012    | OP_Return
        upvalues:  local 1,
0020    | OP_Nil
0021   17 OP_Get_local 3
0023    | OP_Set_global g
0025    | OP_Pop
0026   18 OP_Pop
0027    | OP_Pop
0028    | OP_Close_upvalue
0029   19 OP_Get_global f
0031    | OP_Call 0
0033    | OP_Pop
0034   20 OP_Get_global g
0036    | OP_Call 0
0038    | OP_Pop
0039   21 OP_Nil
0040    | OP_Return
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
f();
g();
)"};
  std::string expected{R"("local"
"after f"
"after f"
"after g"
)"};
  CHECK_EQ(run(source), expected);
}
