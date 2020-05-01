#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("block: empty") {
  const std::string source{R"(
{}
if (true) {}
if (false) {} else {}
print "ok";
)"};
  const std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("block: scope") {
  const std::string source{R"(
var a = "outer";
{
  var a = "inner";
  print a;
}
print a;
)"};
  const std::string expected{R"(inner
outer
)"};
  CHECK_EQ(run(source), expected);
}
