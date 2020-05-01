#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("precedence") {
  std::string source{R"(
print 2 + 3 * 4;
print 20 - 3 * 4;
print 2 + 6 / 3;
print 2 - 6 / 3;
print false == 2 < 1;
print false == 1 > 2;
print false == 2 <= 1;
print false == 1 >= 2;
print 1 - 1;
print 1 -1;
print 1- 1;
print 1-1;
print (2 * (6 - (2 + 2)));
)"};
  std::string expected{R"(14.000000
8.000000
4.000000
0.000000
true
true
true
true
0.000000
0.000000
0.000000
0.000000
4.000000
)"};
  CHECK_EQ(run(source), expected);
}
