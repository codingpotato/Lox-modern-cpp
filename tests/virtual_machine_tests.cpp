#include <doctest/doctest.h>

#include <chrono>
#include <sstream>
#include <string>

#include "helper.h"

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

TEST_CASE("function call") {
  std::string source{R"(
fun add(a, b) { 
  return a + b; 
}
print add(1, 2);
)"};
  std::string expected{R"(3.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("function call with incorrect number of argumentws") {
  const std::string source{R"(
fun a() { b(); }
fun b() { c(); }
fun c() {
  c("too", "many");
}
a();
)"};
  const std::string expected{R"(Expected 0 arguments but got 2.
[line 0005 in] <function: c>
[line 0003 in] <function: b>
[line 0002 in] <function: a>
[line 0007 in] <script>
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("fib") {
  const std::string source{R"(
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}
print fib(30);
)"};
  const std::string expected{R"(832040.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("sum function call") {
  const std::string source{R"(
fun sum(a, b, c) {
  return a + b + c;
}
print 4 + sum(5, 6, 7);
)"};
  const std::string expected{R"(22.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("native clock") {
  const std::string source{R"(
print clock();
)"};
  REQUIRE_EQ(std::stod(run(source)),
             doctest::Approx(
                 static_cast<double>(
                     std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::steady_clock::now().time_since_epoch())
                         .count()) /
                 std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::seconds{1})
                     .count()));
}
