#include <doctest/doctest.h>

#include <string>

#include "chunk.h"

TEST_CASE("chunk emplace back") {
  lox::chunk chunk;
  auto constant = chunk.add_constant(1.2);
  chunk.add_instruction(123, lox::op_constant{}, constant);
  chunk.add_instruction(123, lox::op_return{});
  std::string expected = R"(== test chunk ==
0000  123 op_constant 1.200000
0001    | op_return
)";
  CHECK_EQ(to_string(chunk, "test chunk"), expected);
}