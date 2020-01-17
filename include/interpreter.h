#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include <iostream>
#include <stack>

#include "exception.h"
#include "expression.h"
#include "program.h"
#include "statement.h"
#include "types.h"
#include "value.h"

namespace lox {

class interpreter {
 public:
  struct block_info {
    block_info(statement_id c, statement_id l) noexcept : current{c}, last{l} {}
    statement_id current;
    statement_id last;
  };
  using block_stack = std::stack<block_info>;

  void execute(const program& prog) const {
    block_stack stack;
    if (const auto& start_statement = prog.statements.get(prog.start_block);
        start_statement.storage.is_type<statement::block>()) {
      const auto& block = start_statement.storage.get<statement::block>();
      stack.emplace(block.first, block.last);

      while (!stack.empty()) {
        auto info = stack.top();
        auto current = info.current;
        auto last = info.last;
        stack.pop();
        while (current < last) {
          const auto& statement = prog.statements.get(current);
          statement.storage.visit(overloaded{
              [&stack, &current, &last](const statement::block& block) {
                stack.emplace(current, last);
                current = block.first;
                last = block.last;
              },
              [this, &prog](const statement::expression_s& expr) {
                evaluate(prog, prog.expressions.get(expr.expr));
              },
              [](const statement::for_s&) {},
              [](const statement::function&) {},
              [](const statement::if_else&) {},
              [](const statement::return_s&) {},
              [](const statement::variable_s&) {},
              [](const statement::while_s&) {},
          });
        }
      }
    } else {
      throw internal_error{"Start statement shall be a block."};
    }
  }

  value evaluate(const program& prog, const expression& expr) const noexcept {
    std::cout << "evaluate expr\n";
    return expr.storage.visit(
        overloaded{[this, &prog](const auto& e) { return evaluate(prog, e); }});
  }

  value evaluate(const program& prog, const expression::binary& binary) const
      noexcept {
    std::cout << "evaluate binary\n";
    return evaluate(prog, prog.expressions.get(binary.left)) +
           evaluate(prog, prog.expressions.get(binary.right));
    std::cout << "after evaluate binary\n";
  }

  value evaluate(const program&, const expression::literal&) const noexcept {
    std::cout << "evaluate literal\n";
    return 1;
  }
};

}  // namespace lox

#endif
