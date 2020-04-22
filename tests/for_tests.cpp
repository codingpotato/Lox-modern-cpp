#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("scope") {
  std::string source{R"(
{
  var i = "before";
  for (var i = 0; i < 1; i = i + 1) {
    print i;
    var i = -1;
    print i;
  }
}
{
  for (var i = 0; i > 0; i = i + 1) {}
  var i = "after";
  print i;
  for (i = 0; i < 1; i = i + 1) {
    print i;
  }
}
)"};
  std::string expected{R"(0.000000
-1.000000
after
0.000000
)"};
  CHECK_EQ(run(source), expected);
}
