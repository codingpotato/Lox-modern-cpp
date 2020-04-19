#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("assignment syntax") {
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

TEST_CASE("global assignment") {
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

TEST_CASE("local assignment") {
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