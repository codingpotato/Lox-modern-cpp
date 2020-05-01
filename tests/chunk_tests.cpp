#include <doctest/doctest.h>

#include <string>

#include "chunk.h"
#include "instruction.h"

TEST_CASE("chunk: add instructions") {
  lox::Chunk chunk;
  auto constant = chunk.add_constant(1.2);
  chunk.add<lox::instruction::Constant>(constant, 123);
  chunk.add<lox::instruction::Return>(123);
  std::string expected = R"(== test ==
0000  123 OP_Constant 1.200000
0002    | OP_Return
)";
  CHECK_EQ(to_string(chunk, "test"), expected);
}