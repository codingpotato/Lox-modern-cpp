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
    while (!vm.is_current_block_done()) {
      execute(prog, prog.statements.get(vm.current_statement()));
      vm.advance();
      if (vm.is_current_block_done()) {
        vm.end_scope();
        vm.pop_block();
        if (!vm.is_current_block_done() &&
            !prog.statements.get(vm.current_statement())
                 .storage.is_type<statement::while_s>()) {
          vm.advance();
        }
      }
    }
  }

  void execute(const program& prog, const statement& stat) noexcept {
    stat.storage.visit(overloaded{
        [this, &prog](const auto& element) { execute(prog, element); },
    });
  }

  void execute(const program&, const statement::block& block) noexcept {
    vm.begin_scope();
    vm.excute_block(block.first, block.last);
  }

  void execute(const program& prog,
               const statement::expression_s& expr) noexcept {
    evaluate(prog, prog.expressions.get(expr.expr));
  }

  void execute(const program&, const statement::function&) noexcept {}

  void execute(const program&, const statement::if_else&) noexcept {}

  void execute(const program& prog,
               const statement::print_s& print_s) noexcept {
    out << to_string(evaluate(prog, prog.expressions.get(print_s.value)));
  }

  void execute(const program&, const statement::return_s&) noexcept {}

  void execute(const program& prog,
               const statement::variable_s& variable_s) noexcept {
    vm.define_value(
        evaluate(prog, prog.expressions.get(variable_s.initializer)));
  }

  void execute(const program& prog,
               const statement::while_s& while_s) noexcept {
    const auto condition =
        evaluate(prog, prog.expressions.get(while_s.condition));
    if (condition.as<bool>()) {
      execute(prog, prog.statements.get(while_s.body));
    }
  }

  value evaluate(const program& prog, const expression& expr) const noexcept {
    return expr.storage.visit(
        overloaded{[this, &prog](const auto& e) { return evaluate(prog, e); }});
  }

  value evaluate(const program& prog, const expression::binary& binary) const
      noexcept {
    const auto left = evaluate(prog, prog.expressions.get(binary.left));
    const auto right = evaluate(prog, prog.expressions.get(binary.right));
    switch (binary.oper) {
      case expression::operator_t::plus:
        return left + right;
      case expression::operator_t::minus:
        return left - right;
      case expression::operator_t::multiple:
        return left * right;
      case expression::operator_t::divide:
        return left / right;
      case expression::operator_t::equal:
        return left == right;
      case expression::operator_t::not_equal:
        return left != right;
      case expression::operator_t::great:
        return left > right;
      case expression::operator_t::great_equal:
        return left >= right;
      case expression::operator_t::less:
        return left < right;
      case expression::operator_t::less_equal:
        return left <= right;
      case expression::operator_t::logic_or:
        return left || right;
      case expression::operator_t::logic_and:
        return left && right;
      default:
        throw runtime_error("Unknown operator.");
    }
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

  value evaluate(const program&, const expression::variable& variable) const
      noexcept {
    return vm.get(variable.info);
  }

 private:
  virtual_machine vm;
  std::ostream& out;
};

}  // namespace lox

#endif
