#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("operator: add") {
  SUBCASE("add bool nil") {
    const std::string source{R"(
true + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add bool number") {
    const std::string source{R"(
true + 123;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add bool string") {
    const std::string source{R"(
true + "s";
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add nil nil") {
    const std::string source{R"(
nil + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add number nil") {
    const std::string source{R"(
1 + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add string nil") {
    const std::string source{R"(
"s" + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add") {
    const std::string source{R"(
print 123 + 456;
print "str" + "ing";
)"};
    const std::string expected{R"(579.000000
string
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: comparison") {
  std::string source{R"(
print 1 < 2;
print 2 < 2;
print 2 < 1;
print 1 <= 2;
print 2 <= 2;
print 2 <= 1;
print 1 > 2;
print 2 > 2;
print 2 > 1;
print 1 >= 2;
print 2 >= 2;
print 2 >= 1;
print 0 < -0;
print -0 < 0;
print 0 > -0;
print -0 > 0;
print 0 <= -0;
print -0 <= 0;
print 0 >= -0;
print -0 >= 0;
)"};
  std::string expected{R"(true
false
false
true
true
false
false
false
true
false
true
true
false
false
false
false
true
true
true
true
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("operator: divide") {
  SUBCASE("divide non-number numebr") {
    const std::string source{R"(
"1" / 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add bool number") {
    const std::string source{R"(
true + 123;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add bool string") {
    const std::string source{R"(
true + "s";
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add nil nil") {
    const std::string source{R"(
nil + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add number nil") {
    const std::string source{R"(
1 + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add string nil") {
    const std::string source{R"(
"s" + nil;
)"};
    const std::string expected{R"(Operands must be two numbers or two strings.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("add") {
    const std::string source{R"(
print 123 + 456;
print "str" + "ing";
)"};
    const std::string expected{R"(579.000000
string
)"};
    CHECK_EQ(run(source), expected);
  }
}
