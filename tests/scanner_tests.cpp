#include <doctest.h>

#include <string>

#include "scanner.h"

TEST_CASE("") {
  lox::scanner scanner{"print 1"};
  auto tokens = scanner.scan();
  CHECK_EQ(tokens.size(), 3);
}