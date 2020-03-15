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

  source = {R"(
if (false) {
  print 1;
} else {
  print 2;
}
)"};
  expected = {"2\n"};
  CHECK_EQ(run(source), expected);
}
