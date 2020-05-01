#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("call: only call functions and classes") {
  SUBCASE("bool") {
    const std::string source{R"(
true();
)"};
    const std::string expected{R"(Can only call functions and classes.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("nil") {
    const std::string source{R"(
nil();
)"};
    const std::string expected{R"(Can only call functions and classes.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("number") {
    const std::string source{R"(
123();
)"};
    const std::string expected{R"(Can only call functions and classes.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("string") {
    const std::string source{R"(
"string"();
)"};
    const std::string expected{R"(Can only call functions and classes.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }
}
