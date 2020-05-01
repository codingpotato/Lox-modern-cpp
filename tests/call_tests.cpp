#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("call: bool") {
  const std::string source{R"(
true();
)"};
  const std::string expected{R"(Can only call functions and classes.
[line 0002 in] <script>
)"};
  CHECK_EQ(run(source), expected);
}
