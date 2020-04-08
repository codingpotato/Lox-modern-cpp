#include <doctest/doctest.h>

#include "opcode.h"

TEST_CASE("opcode") {
  lox::OP_constant op_constant;
  CHECK_EQ(op_constant.id, 0);
}
