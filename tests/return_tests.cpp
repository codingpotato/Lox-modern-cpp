#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("return: after else") {
  std::string source{R"(
fun f() {
  if (false) "no"; else return "ok";
}
print f();
)"};
  std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("return: after if") {
  std::string source{R"(
fun f() {
  if (true) return "ok";
}
print f();
)"};
  std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("return: after while") {
  std::string source{R"(
fun f() {
  while (true) return "ok";
}
print f();
)"};
  std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("return: at top level") {
  std::string source{R"(
return "wat";
)"};
  std::string expected{
      R"([line 2] Error at 'return': Cannot return from top-level code.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("return: in function") {
  std::string source{R"(
fun f() {
  return "ok";
  print "bad";
}
print f();
)"};
  std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("return: return nil if no value") {
  std::string source{R"(
fun f() {
  return;
  print "bad";
}
print f();
)"};
  std::string expected{R"(nil
)"};
  CHECK_EQ(run(source), expected);
}
