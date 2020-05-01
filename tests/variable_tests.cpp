#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("variable: collide with paramter") {
  const std::string source{R"(
fun foo(a) {
  var a;
}
)"};
  const std::string expected{
      R"([line 3] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate local") {
  const std::string source{R"(
{
  var a = "value";
  var a = "other";
}
)"};
  const std::string expected{
      R"([line 4] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate parameter") {
  const std::string source{R"(
fun foo(arg,
        arg) {
  "body";
}
)"};
  const std::string expected{
      R"([line 3] Error at 'arg': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: early bound") {
  const std::string source{R"(
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
  const std::string expected{R"(outer
outer
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: in middle of block") {
  const std::string source{R"(
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
  const std::string expected{R"(a
a b
a c
a b d
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: in nested block") {
  const std::string source{R"(
{
  var a = "outer";
  {
    print a;
  }
}
)"};
  const std::string expected{R"(outer
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: redeclare global") {
  const std::string source{R"(
var a = "21";
var a;
print a;
)"};
  const std::string expected{R"(nil
)"};
  CHECK_EQ(run(source), expected);
}
