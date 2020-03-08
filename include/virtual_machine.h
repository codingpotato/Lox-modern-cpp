#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <iostream>
#include <string>
#include <vector>

#include "chunk.h"
#include "exception.h"
#include "instruction.h"
#include "value.h"

#define DEBUG_TRACE_EXECUTION

namespace lox {

struct virtual_machine {
  void interpret(chunk ch) {
    main_ = std::move(ch);
    stack_.clear();
    for (auto& instr : main_.code()) {
#ifdef DEBUG_TRACE_EXECUTION
      std::cout << instr.repr([](const auto&) -> std::string { return ""; });
#endif

      switch (instr.opcode()) {
        case instruction::op_constant:
          std::cout << main_.constants()[instr.oprand()] << "\n";
          break;
        case instruction::op_return:
          break;
        default:
          throw internal_error{"Unknown opcode."};
      }
    }
  }

 private:
  chunk main_;
  std::vector<value> stack_;
};

}  // namespace lox

#endif
