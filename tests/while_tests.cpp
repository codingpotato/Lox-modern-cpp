#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("syntax") {
  std::string source{R"(
var c = 0;
while (c < 3) print c = c + 1;

var a = 0;
while (a < 3) {
  print a;
  a = a + 1;
}

while (false) if (true) 1; else 2;
while (false) while (true) 1;
while (false) for (;;) 1;
)"};
  std::string expected{R"(1.000000
2.000000
3.000000
0.000000
1.000000
2.000000
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("closure in body") {
  std::string source{R"(
var f1;
var f2;
var f3;

var i = 1;
while (i < 4) {
  var j = i;
  fun f() { print j; }

  if (j == 1) f1 = f;
  else if (j == 2) f2 = f;
  else f3 = f;

  i = i + 1;
}

f1();
f2();
f3();
)"};
  std::string expected{R"(1.000000
2.000000
3.000000
)"};
  CHECK_EQ(run(source), expected);
}
