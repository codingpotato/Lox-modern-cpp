#include <doctest/doctest.h>

#include <sstream>
#include <string>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

inline std::string run(const std::string& source) {
  lox::scanner scanner{source};
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  lox::compiler compiler{vm};
  vm.interpret(compiler.compile(scanner.scan()));
  return oss.str();
}

TEST_CASE("binary") {
  const std::string source{"print 1 + 2;"};
  const std::string expected{"3.000000\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("global variable") {
  const std::string source{R"(
var a = 1;
var b = 2;
print a + b;
)"};
  const std::string expected{"3.000000\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("set global variable") {
  const std::string source{R"(
var a = 1;
a = 2;
print a;
)"};
  const std::string expected{"2.000000\n"};
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
  const std::string expected{"1.000000\n"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("if statement") {
  std::string source{R"(
if (true) print 1;
)"};
  std::string expected{"1.000000\n"};
  CHECK_EQ(run(source), expected);

  source = std::string{R"(
if (1 > 2) {
  print 1;
} else {
  print 2;
}
)"};
  expected = std::string{"2.000000\n"};
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
  std::string expected{R"(1.000000
2.000000
3.000000
4.000000
5.000000
6.000000
7.000000
8.000000
9.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for statement") {
  std::string source{R"(
for (var a = 1; a < 10; a = a + 1) {
  print a;
}
)"};
  std::string expected{R"(1.000000
2.000000
3.000000
4.000000
5.000000
6.000000
7.000000
8.000000
9.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("sum") {
  std::string source{R"(
var sum = 0.0;
var i = 0;
while (i < 10000000) {
    sum = sum + i;
    i = i + 1;
}
print sum;
)"};
  std::string expected{R"(49999995000000.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("function declaration") {
  std::string source{R"(
fun areWeHavingItYet() {
  print "Yes we are!";
}
print areWeHavingItYet;
)"};
  std::string expected{R"(<function: areWeHavingItYet>
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("function call with incorrect number of argumentws") {
  std::string source{R"(
fun a() { b(); }
fun b() { c(); }
fun c() {
  c("too", "many");
}
a();
)"};
  std::string expected{R"(Expected 0 arguments but got 2.
<function: c>
<function: b>
<function: a>
<script>
)"};
  CHECK_EQ(run(source), expected);
}
