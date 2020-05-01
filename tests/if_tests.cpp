#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("if: class in else") {
  std::string source{R"(
if (true) "ok"; else class Foo {}
)"};
  std::string expected{R"([line 2] Error at 'class': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: class in then") {
  std::string source{R"(
if (true) class Foo {}
)"};
  std::string expected{R"([line 2] Error at 'class': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: danging else") {
  std::string source{R"(
if (true) if (false) print "bad"; else print "good";
if (false) if (true) print "bad"; else print "bad";
)"};
  std::string expected{R"(good
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: danging else") {
  std::string source{R"(
if (true) print "good"; else print "bad";
if (false) print "bad"; else print "good";
if (false) nil; else { print "block"; }
)"};
  std::string expected{R"(good
good
block
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: function in else") {
  std::string source{R"(
if (true) "ok"; else fun foo() {}
)"};
  std::string expected{R"([line 2] Error at 'fun': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: function in then") {
  std::string source{R"(
if (true) fun foo() {}
)"};
  std::string expected{R"([line 2] Error at 'fun': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: syntax") {
  std::string source{R"(
if (true) print "good";
if (false) print "bad";
if (true) { print "block"; }
var a = false;
if (a = true) print a;
)"};
  std::string expected{R"(good
block
true
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: truth") {
  std::string source{R"(
if (false) print "bad"; else print "false";
if (nil) print "bad"; else print "nil";
if (true) print true;
if (0) print 0;
if ("") print "empty";
)"};
  std::string expected{R"(false
nil
true
0.000000
empty
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: var in else") {
  std::string source{R"(
if (true) "ok"; else var foo;
)"};
  std::string expected{R"([line 2] Error at 'var': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if: var in then") {
  std::string source{R"(
if (true) var foo;
)"};
  std::string expected{R"([line 2] Error at 'var': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}
