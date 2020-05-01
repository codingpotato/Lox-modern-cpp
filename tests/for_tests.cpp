#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("for: closure in body") {
  std::string source{R"(
var f1;
var f2;
var f3;
for (var i = 1; i < 4; i = i + 1) {
  var j = i;
  fun f() {
    print i;
    print j;
  }
  if (j == 1) f1 = f;
  else if (j == 2) f2 = f;
  else f3 = f;
}
f1();
f2();
f3();
)"};
  std::string expected{R"(4.000000
1.000000
4.000000
2.000000
4.000000
3.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: function in body") {
  std::string source{R"(
for (;;) fun foo() {}
)"};
  std::string expected{R"([line 2] Error at 'fun': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: return closure") {
  std::string source{R"(
fun f() {
  for (;;) {
    var i = "i";
    fun g() { print i; }
    return g;
  }
}
var h = f();
h();
)"};
  std::string expected{R"(i
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: return inside") {
  std::string source{R"(
fun f() {
  for (;;) {
    var i = "i";
    return i;
  }
}
print f();
)"};
  std::string expected{R"(i
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: scope") {
  std::string source{R"(
{
  var i = "before";
  for (var i = 0; i < 1; i = i + 1) {
    print i;
    var i = -1;
    print i;
  }
}
{
  for (var i = 0; i > 0; i = i + 1) {}
  var i = "after";
  print i;
  for (i = 0; i < 1; i = i + 1) {
    print i;
  }
}
)"};
  std::string expected{R"(0.000000
-1.000000
after
0.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: statement condition") {
  std::string source{R"(
for (var a = 1; {}; a = a + 1) {}
)"};
  std::string expected{R"([line 2] Error at '{': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: statement increment") {
  std::string source{R"(
for (var a = 1; a < 2; {}) {}
)"};
  std::string expected{R"([line 2] Error at '{': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: statement initializer") {
  std::string source{R"(
for ({}; a < 2; a = a + 1) {}
)"};
  std::string expected{R"([line 2] Error at '{': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: syntax") {
  std::string source{R"(
for (var c = 0; c < 3;) print c = c + 1;

for (var a = 0; a < 3; a = a + 1) {
  print a;
}

fun foo() {
  for (;;) return "done";
}
print foo();

var i = 0;
for (; i < 2; i = i + 1) print i;

fun bar() {
  for (var i = 0;; i = i + 1) {
    print i;
    if (i >= 2) return;
  }
}
bar();

for (var i = 0; i < 2;) {
  print i;
  i = i + 1;
}
for (; false;) if (true) 1; else 2;
for (; false;) while (true) 1;
for (; false;) for (;;) 1;
)"};
  std::string expected{R"(1.000000
2.000000
3.000000
0.000000
1.000000
2.000000
done
0.000000
1.000000
0.000000
1.000000
2.000000
0.000000
1.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("for: var in body") {
  std::string source{R"(
for (;;) var foo;
)"};
  std::string expected{R"([line 2] Error at 'var': Expect expression.
)"};
  CHECK_EQ(run(source), expected);
}
