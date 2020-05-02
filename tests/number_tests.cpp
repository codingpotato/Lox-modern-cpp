#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("number") {
  SUBCASE("decimal point at eof") {
    const std::string source{R"(
123.
)"};
    const std::string expected{R"([line 2] Error at '.': Expect ';' after value.
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("leading dot") {
    const std::string source{R"(
.123
)"};
    const std::string expected{R"([line 2] Error at '.': Expect expression.
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("literals") {
    const std::string source{R"(
print 123;
print 987654;
print 0;
print -0;
print 123.456;
print -0.001;
)"};
    const std::string expected{R"(123.000000
987654.000000
0.000000
-0.000000
123.456000
-0.001000
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("nan equality") {
    const std::string source{R"(
var nan = 0/0;
print nan == 0; // expect: false
print nan != 1; // expect: true
print nan == nan; // expect: false
print nan != nan; // expect: true
)"};
    const std::string expected{R"(false
true
false
true
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("trailing dot") {
    const std::string source{R"(
123.;
)"};
    const std::string expected{R"([line 2] Error at '.': Expect ';' after value.
)"};
    CHECK_EQ(run(source), expected);
  }
}
