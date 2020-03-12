#include <doctest.h>

#include <string>

#include "chunk.h"

TEST_CASE("chunk emplace back") {
  lox::chunk chunk;
  auto constant = chunk.add_constant(1.2);
  chunk.add_instruction(lox::op_constant{}, constant, 123);
  chunk.add_instruction(lox::op_return{}, 123);
  std::string expected = R"(== test chunk ==
0000  123 op_constant 1.2
0001    | op_return
)";
  CHECK_EQ(chunk.repr("test chunk"), expected);
}