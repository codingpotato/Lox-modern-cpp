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
      std::cout << "    ";
      for (auto& v : stack_) {
        std::cout << "[ " << v.as<double>() << " ]";
      }
      std::cout << "\n";
#endif

      switch (instr.opcode()) {
        case instruction::op_constant:
          push(main_.constants()[instr.oprand()]);
          break;
        case instruction::op_add: {
          auto b = pop();
          auto a = pop();
          push(a.as<double>() + b.as<double>());
          break;
        }
        case instruction::op_subtract: {
          auto b = pop();
          auto a = pop();
          push(a.as<double>() - b.as<double>());
          break;
        }
        case instruction::op_multiply: {
          auto b = pop();
          auto a = pop();
          push(a.as<double>() * b.as<double>());
          break;
        }
        case instruction::op_divide: {
          auto b = pop();
          auto a = pop();
          push(a.as<double>() / b.as<double>());
          break;
        }
        case instruction::op_negate:
          push(-pop().as<double>());
          break;
        case instruction::op_return:
          ENSURES(stack_.size() == 1);
          std::cout << pop().as<double>() << "\n";
          break;
        default:
          throw internal_error{"Unknown opcode."};
      }
    }
  }

 private:
  void push(value v) noexcept { stack_.emplace_back(std::move(v)); }
  value pop() noexcept {
    auto v = stack_.back();
    stack_.pop_back();
    return v;
  }

  chunk main_;
  std::vector<value> stack_;
};

}  // namespace lox

#endif
