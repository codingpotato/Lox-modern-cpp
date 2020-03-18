#include <doctest.h>

#include "vector.h"

TEST_CASE("vector") {
  lox::vector<int> vector;
  vector.emplace_back(1);
  CHECK_EQ(vector.size(), 1);
  CHECK_EQ(vector[0], 1);
}