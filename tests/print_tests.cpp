#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("print: missing argument") {
  std::string source{R"(
print;
)"};
  std::string expected{R"([line 2] Error at ';': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}
