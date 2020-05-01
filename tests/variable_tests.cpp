#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("variable: collide with paramter") {
  const std::string source{R"(
fun foo(a) {
  var a;
}
)"};
  const std::string expected{
      R"([line 3] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate local") {
  const std::string source{R"(
{
  var a = "value";
  var a = "other";
}
)"};
  const std::string expected{
      R"([line 4] Error at 'a': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: duplicate parameter") {
  const std::string source{R"(
fun foo(arg,
        arg) {
  "body";
}
)"};
  const std::string expected{
      R"([line 3] Error at 'arg': Variable with this name already declared in this scope.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: early bound") {
  const std::string source{R"(
var a = "outer";
{
  fun foo() {
    print a;
  }
  foo();
  var a = "inner";
  foo();
}
)"};
  const std::string expected{R"(outer
outer
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: in middle of block") {
  const std::string source{R"(
{
  var a = "a";
  print a;
  var b = a + " b";
  print b;
  var c = a + " c";
  print c;
  var d = b + " d";
  print d;
}
)"};
  const std::string expected{R"(a
a b
a c
a b d
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: in nested block") {
  const std::string source{R"(
{
  var a = "outer";
  {
    print a;
  }
}
)"};
  const std::string expected{R"(outer
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: redeclare global") {
  const std::string source{R"(
var a = "21";
var a;
print a;
)"};
  const std::string expected{R"(nil
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: redefine global") {
  const std::string source{R"(
var a = "1";
var a = "2";
print a;
)"};
  const std::string expected{R"(2
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: scope reuse in different blocks") {
  const std::string source{R"(
{
  var a = "first";
  print a;
}
{
  var a = "second";
  print a;
}
)"};
  const std::string expected{R"(first
second
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: shadow and local") {
  const std::string source{R"(
{
  var a = "outer";
  {
    print a;
    var a = "inner";
    print a;
  }
}
)"};
  const std::string expected{R"(outer
inner
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: shadow global") {
  const std::string source{R"(
var a = "global";
{
  var a = "shadow";
  print a;
}
print a;
)"};
  const std::string expected{R"(shadow
global
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: shadow local") {
  const std::string source{R"(
{
  var a = "local";
  {
    var a = "shadow";
    print a;
  }
  print a;
}
)"};
  const std::string expected{R"(shadow
local
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: undefined global") {
  const std::string source{R"(
print notDefined;
)"};
  const std::string expected{R"(Undefined variable: 'notDefined'.
[line 0002] in <script>
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: undefined local") {
  const std::string source{R"(
{
  print notDefined;
}
)"};
  const std::string expected{R"(Undefined variable: 'notDefined'.
[line 0003] in <script>
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: uninitialized") {
  const std::string source{R"(
var a;
print a;
)"};
  const std::string expected{R"(nil
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: unreached undefined") {
  const std::string source{R"(
if (false) {
  print notDefined;
}
print "ok";
)"};
  const std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: use false as var") {
  const std::string source{R"(
var false = "value";
)"};
  const std::string expected{R"([line 2] Error at 'false': Expect variable name.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: use global in initializer") {
  const std::string source{R"(
var a = "value";
var a = a;
print a;
)"};
  const std::string expected{R"(value
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: use local in initializer") {
  const std::string source{R"(
var a = "outer";
{
  var a = a;
}
)"};
  const std::string expected{
      R"([line 4] Error at 'a': Cannot read local variable in its own initializer.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: use nil as var") {
  const std::string source{R"(
var nil = "value";
)"};
  const std::string expected{R"([line 2] Error at 'nil': Expect variable name.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("variable: use this as var") {
  const std::string source{R"(
var this = "value";
)"};
  const std::string expected{R"([line 2] Error at 'this': Expect variable name.
)"};
  CHECK_EQ(run(source), expected);
}
