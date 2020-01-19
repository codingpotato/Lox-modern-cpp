#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "exception.h"
#include "expression.h"
#include "program.h"
#include "statement.h"
#include "types.h"
#include "value.h"
#include "virtual_machine.h"

namespace lox {

class interpreter {
 public:
  void execute(const program& prog) noexcept {
    execute(prog, prog.statements.get(prog.start_block));
    while (true) {
      while (vm.ip.current < vm.ip.last) {
        execute(prog, prog.statements.get(vm.ip.current));
        ++vm.ip.current;
      }
      if (vm.stack_frame.empty()) {
        break;
      }
      vm.ip = vm.stack_frame.top();
      vm.stack_frame.pop();
    }
  }

  void execute(const program& prog, const statement& stat) noexcept {
    stat.storage.visit(overloaded{
        [this, &prog](const auto& s) { execute(prog, s); },
    });
  }

  void execute(const program& prog, const statement::block& block) noexcept {
    if (vm.ip.current != statement_id{}) {
      if (const auto& current = prog.statements.get(vm.ip.current);
          !current.storage.is_type<statement::while_s>()) {
        ++vm.ip.current;
      }
      vm.stack_frame.push(vm.ip);
    }
    vm.ip = {block.first, block.last};
  }

  void execute(const program& prog,
               const statement::expression_s& expr) noexcept {
    evaluate(prog, prog.expressions.get(expr.expr));
  }

  void execute(const program&, const statement::function&) noexcept {}

  void execute(const program&, const statement::if_else&) noexcept {}

  void execute(const program&, const statement::return_s&) noexcept {}

  void execute(const program&, const statement::variable_s&) noexcept {}

  void execute(const program&, const statement::while_s&) noexcept {}

  value evaluate(const program& prog, const expression& expr) const noexcept {
    return expr.storage.visit(
        overloaded{[this, &prog](const auto& e) { return evaluate(prog, e); }});
  }

  value evaluate(const program& prog, const expression::binary& binary) const
      noexcept {
    return evaluate(prog, prog.expressions.get(binary.left)) +
           evaluate(prog, prog.expressions.get(binary.right));
  }

  value evaluate(const program&, const expression::literal&) const noexcept {
    return 1;
  }

 private:
  virtual_machine vm;
};

}  // namespace lox

#endif
