#include <doctest/doctest.h>

#include <string>

#include "chunk.h"
#include "instruction.h"
#include "object.h"

TEST_CASE("chunk") {
  lox::Chunk chunk;

  const auto constant = chunk.add_constant(1.2);
  chunk.add<lox::instruction::Constant>(constant, 1);

  lox::instruction::Closure::Upvalue_vector upvalues{{100, 0}, {200, 1}};
  lox::Function func;
  func.upvalue_count = upvalues.size();
  const auto func_index = chunk.add_constant(&func);
  chunk.add<lox::instruction::Closure>(func_index, upvalues, 2);

  const auto jump = chunk.add<lox::instruction::Jump>(0, 3);
  chunk.patch_jump(jump, 5);

  chunk.add<lox::instruction::Return>(4);

  const std::string expected = R"(== test ==
0000    1 OP_Constant 1.200000
0002    2 OP_Closure <script>
        upvalues:  upvalue 100, local 200,
0008    3 OP_Jump 5 -> 16
0011    4 OP_Return
)";
  REQUIRE_EQ(to_string(chunk, "test"), expected);
}
