#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("variable: collide with paramter") {
  std::string source{R"(
fun foo(a) {
  var a;
}
)"};
  std::string expected{
      R"([line 3] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate local") {
  std::string source{R"(
{
  var a = "value";
  var a = "other";
}
)"};
  std::string expected{
      R"([line 4] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate parameter") {
  std::string source{R"(
fun foo(arg,
        arg) {
  "body";
}
)"};
  std::string expected{
      R"([line 3] Error at 'arg': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: early bound") {
  std::string source{R"(
var a = "outer";
{
  fun foo() {
    print a;
  }
  foo();
  var a = "inner";
  foo();
}
)"};
  std::string expected{R"(outer
outer
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: in middle of block") {
  std::string source{R"(
{
  var a = "a";
  print a;
  var b = a + " b";
  print b;
  var c = a + " c";
  print c;
  var d = b + " d";
  print d;
}
)"};
  std::string expected{R"(a
a b
a c
a b d
)"};
  CHECK_EQ(run(source), expected);
}
