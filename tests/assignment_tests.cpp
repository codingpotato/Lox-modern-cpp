#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("assignment: associativity") {
  const std::string source{R"(
var a = "a";
var b = "b";
var c = "c";
a = b = c;
print a;
print b;
print c;
)"};
  const std::string expected{R"(c
c
c
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("assignment: global") {
  const std::string source{R"(
var a = "before";
print a;
a = "after";
print a;
print a = "arg";
print a;
)"};
  const std::string expected{R"(before
after
arg
arg
)"};
  CHECK_EQ(run(source), expected);
}

// todo: shall throw
TEST_CASE("assignment: grouping") {
  const std::string source{R"(
var a = "a";
(a) = "value"; // Error at '=': Invalid assignment target.
)"};
  const std::string expected{R"()"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("assignment: local") {
  const std::string source{R"({
    var a = "before";
    print a;
    a = "after";
    print a;
    print a = "arg";
    print a;
}
)"};
  const std::string expected{R"(before
after
arg
arg
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("assignment: syntax") {
  const std::string source{R"(
var a = "before";
var c = a = "var";
print a;
print c;
)"};
  const std::string expected{R"(var
var
)"};
  CHECK_EQ(run(source), expected);
}
