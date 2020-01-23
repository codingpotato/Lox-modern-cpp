#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include <ostream>

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
  explicit interpreter(std::ostream& o) noexcept : out{o} {}

  void execute(const program& prog) noexcept {
    execute(prog, prog.statements.get(prog.start_block));
    while (!vm.done()) {
      while (vm.is_in_current_block()) {
        execute(prog, prog.statements.get(vm.current_statement()));
        vm.next_statement();
      }
      vm.pop_block();
    }
  }

  void execute(const program& prog, const statement& stat) noexcept {
    stat.storage.visit(overloaded{
        [this, &prog](const auto& s) { execute(prog, s); },
    });
  }

  void execute(const program& prog, const statement::block& block) noexcept {
    const auto advance_current =
        !vm.done() && !prog.statements.get(vm.current_statement())
                           .storage.is_type<statement::while_s>();
    vm.excute_block(block.first, block.last, advance_current);
  }

  void execute(const program& prog,
               const statement::expression_s& expr) noexcept {
    evaluate(prog, prog.expressions.get(expr.expr));
  }

  void execute(const program&, const statement::function&) noexcept {}

  void execute(const program&, const statement::if_else&) noexcept {}

  void execute(const program& prog,
               const statement::print_s& print_s) noexcept {
    out << evaluate(prog, prog.expressions.get(print_s.value)).as<int>();
  }

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

  value evaluate(const program& prog, const expression::literal& literal) const
      noexcept {
    return literal.storage.visit(overloaded{
        [](null) { return value{}; },
        [](bool b) { return value{b}; },
        [](int i) { return value{i}; },
        [&prog](double_id d) { return value{prog.double_literals.get(d)}; },
        [](string_id) {
          return value{}; /* todo*/
        },
    });
  }

 private:
  virtual_machine vm;
  std::ostream& out;
};

}  // namespace lox

#endif
