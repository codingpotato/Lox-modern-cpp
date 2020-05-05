#include <doctest/doctest.h>

#include <string>

#include "heap.h"

TEST_CASE("heap: make string") {
  lox::Heap heap;
  std::string str{"string"};
  CHECK_EQ(heap.make_string(str), heap.make_string(str));
}
