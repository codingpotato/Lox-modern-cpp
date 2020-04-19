#include <doctest/doctest.h>

#include "helper.h"

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

TEST_CASE("assign to shadowed later") {
  std::string source{R"(
var a = "global";
{
  fun assign() {
    a = "assigned";
  }
  var a = "inner";
  assign();
  print a;
}
print a;
)"};
  std::string expected{R"("inner"
"assigned"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("close over function parameter") {
  std::string source{R"(
var f;
fun foo(param) {
  fun f_() {
    print param;
  }
  f = f_;
}
foo("param");
f();
)"};
  std::string expected{R"("param"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("close over later variable") {
  std::string source{R"(
fun f() {
  var a = "a";
  var b = "b";
  fun g() {
    print b;
    print a;
  }
  g();
}
f();
)"};
  std::string expected{R"("b"
"a"
)"};
  CHECK_EQ(run(source), expected);
}
