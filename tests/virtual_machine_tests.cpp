#include <doctest.h>

#include <sstream>
#include <string>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

inline std::string run(const std::string& source) {
  lox::scanner scanner{source};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  std::ostringstream oss;
  lox::virtual_machine{oss}.interpret(chunk);
  return oss.str();
}

TEST_CASE("binary") {
  const std::string source{"print 1 + 2;"};
  const std::string expected{"3\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("global variable") {
  const std::string source{R"(
var a = 1;
var b = 2;
print a + b;
)"};
  const std::string expected{"3\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("set global variable") {
  const std::string source{R"(
var a = 1;
a = 2;
print a;
)"};
  const std::string expected{"2\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("string variable") {
  const std::string source{R"(
var breakfast = "beignets";
var beverage = "cafe au lait";
breakfast = "beignets with " + beverage;
print breakfast;
)"};
  const std::string expected{R"("beignets with ""cafe au lait"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("local variable") {
  const std::string source{R"(
{
  var a = 1;
  {
    print a;
  }
}
)"};
  const std::string expected{"1\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if statement") {
  std::string source{R"(
if (true) print 1;
)"};
  std::string expected{"1\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
if (1 > 2) {
  print 1;
} else {
  print 2;
}
)"};
  expected = std::string{"2\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("and expression") {
  std::string source{R"(
print true and true;
)"};
  std::string expected{"true\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print true and false;
)"};
  expected = std::string{"false\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print false and true;
)"};
  expected = std::string{"false\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print false and false;
)"};
  expected = std::string{"false\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("or expression") {
  std::string source{R"(
print true or true;
)"};
  std::string expected{"true\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print true or false;
)"};
  expected = std::string{"true\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print false or true;
)"};
  expected = std::string{"true\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
print false or false;
)"};
  expected = std::string{"false\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("while statement") {
  std::string source{R"(
var a = 1;
while (a < 10) {
  print a;
  a = a + 1;
}
)"};
  std::string expected{R"(1
2
3
4
5
6
7
8
9
)"};
  CHECK_EQ(run(source), expected);
}
