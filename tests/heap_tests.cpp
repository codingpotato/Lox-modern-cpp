#include <doctest/doctest.h>

#include <string>

#include "heap.h"

TEST_CASE("heap") {
  lox::heap heap;
  std::string str{"test string"};
  auto obj = heap.add_string(str);
  (void)obj;
  CHECK_EQ(heap.add_string(str), obj);
}
