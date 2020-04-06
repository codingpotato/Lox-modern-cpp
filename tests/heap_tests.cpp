#include <doctest/doctest.h>

#include <string>

#include "heap.h"

TEST_CASE("heap") {
  lox::Heap heap;
  std::string str{"test string"};
  auto obj = heap.make_string(str);
  (void)obj;
  CHECK_EQ(heap.make_string(str), obj);
}
