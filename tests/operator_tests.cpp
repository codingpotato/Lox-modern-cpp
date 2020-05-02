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

  SUBCASE("divide number non-number") {
    const std::string source{R"(
1 / "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("divide") {
    const std::string source{R"(
print 8 / 2;
print 12.34 / 12.34;
)"};
    const std::string expected{R"(4.000000
1.000000
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: equals") {
  const std::string source{R"(
print nil == nil;
print true == true;
print true == false;
print 1 == 1;
print 1 == 2;
print "str" == "str";
print "str" == "ing";
print nil == false;
print false == 0;
print 0 == "0";
)"};
  const std::string expected{R"(true
true
false
true
false
true
false
false
false
false
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("operator: greater") {
  SUBCASE("greater non-number numebr") {
    const std::string source{R"(
"1" > 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("greater number non-number") {
    const std::string source{R"(
1 > "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("greater or equal non-number numebr") {
    const std::string source{R"(
"1" >= 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("greater or equal number non-number") {
    const std::string source{R"(
1 >= "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: less") {
  SUBCASE("less non-number numebr") {
    const std::string source{R"(
"1" < 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("less number non-number") {
    const std::string source{R"(
1 < "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("less or equal non-number numebr") {
    const std::string source{R"(
"1" <= 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("less or equal number non-number") {
    const std::string source{R"(
1 <= "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: multiply") {
  SUBCASE("multiply non-number numebr") {
    const std::string source{R"(
"1" * 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("multiply number non-number") {
    const std::string source{R"(
1 * "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("multiply") {
    const std::string source{R"(
print 5 * 3;
print 12.34 * 0.3;
)"};
    const std::string expected{R"(15.000000
3.702000
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: negate") {
  SUBCASE("negate non-number") {
    const std::string source{R"(
-"s";
)"};
    const std::string expected{R"(Operand must be a number.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("negate") {
    const std::string source{R"(
print -(3); // expect: -3
print --(3); // expect: 3
print ---(3); // expect: -3
)"};
    const std::string expected{R"(-3.000000
3.000000
-3.000000
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: not equals") {
  const std::string source{R"(
print nil != nil;
print true != true;
print true != false;
print 1 != 1;
print 1 != 2;
print "str" != "str";
print "str" != "ing";
print nil != false;
print false != 0;
print 0 != "0";
)"};
  const std::string expected{R"(false
false
true
false
true
false
true
true
true
true
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("operator: not") {
  const std::string source{R"(
print !true;
print !false;
print !!true;
print !123;
print !0;
print !nil;
print !"";
fun foo() {}
print !foo;
)"};
  const std::string expected{R"(false
true
true
false
false
true
false
false
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("operator: subtract") {
  SUBCASE("subtract non-number numebr") {
    const std::string source{R"(
"1" - 1;
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("subtract number non-number") {
    const std::string source{R"(
1 - "1";
)"};
    const std::string expected{R"(Operands must be numbers.
[line 0002] in <script>
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("subtract") {
    const std::string source{R"(
print 4 - 3;
print 1.2 - 1.2;
)"};
    const std::string expected{R"(1.000000
0.000000
)"};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("operator: logical") {
  SUBCASE("and truth") {
    const std::string source{R"(
print false and "bad";
print nil and "bad";
print true and "ok";
print 0 and "ok";
print "" and "ok";
)"};
    const std::string expected{R"(false
nil
ok
ok
ok
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("and") {
    const std::string source{R"(
print false and 1;
print true and 1;
print 1 and 2 and false;
print 1 and true;
print 1 and 2 and 3;
var a = "before";
var b = "before";
(a = true) and
    (b = false) and
    (a = "bad");
print a;
print b;
)"};
    const std::string expected{R"(false
1.000000
false
true
3.000000
true
false
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("or truth") {
    const std::string source{R"(
print false or "ok";
print nil or "ok";
print true or "ok";
print 0 or "ok";
print "s" or "ok";
)"};
    const std::string expected{R"(ok
ok
true
0.000000
s
)"};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("or") {
    const std::string source{R"(
print 1 or true;
print false or 1;
print false or false or true;
print false or false;
print false or false or false;
var a = "before";
var b = "before";
(a = false) or
    (b = true) or
    (a = "bad");
print a;
print b;
)"};
    const std::string expected{R"(1.000000
1.000000
true
false
false
false
true
)"};
    CHECK_EQ(run(source), expected);
  }
}
