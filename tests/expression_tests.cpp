#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("expression: evaluate") {
  const std::string source{R"(
print (5 - (3 - 1)) + -1;
)"};
  const std::string expected{R"(2.000000
)"};
  CHECK_EQ(run(source), expected);
}
