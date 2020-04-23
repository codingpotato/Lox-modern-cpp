#include <doctest/doctest.h>

#include "helper.h"

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
