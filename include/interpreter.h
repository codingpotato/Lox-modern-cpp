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
  explicit interpreter(std::ostream& os) noexcept : out{os} {}

  void execute(const program& prog) noexcept {
    execute(prog, prog.statements.get(prog.start_block), false);
    while (!vm.is_current_block_done()) {
      execute(prog, prog.statements.get(vm.current_statement()), true);
      if (vm.is_current_block_done()) {
        vm.end_scope();
        vm.pop_block();
      }
    }
  }

  void execute(const program& prog, const statement& stat,
               bool current_statement_finished) noexcept {
    stat.storage.visit(overloaded{
        [this, &prog](const auto& element) { execute(prog, element); },
        [this, &prog,
         current_statement_finished](const statement::block& block) {
          execute(prog, block, current_statement_finished);
        },
    });
  }

  void execute(const program&, const statement::block& block,
               bool current_statement_finished) noexcept {
    vm.begin_scope();
    vm.excute_block(block.first, block.last, current_statement_finished);
  }

  void execute(const program& prog,
               const statement::expression_s& expr) noexcept {
    evaluate(prog, prog.expressions.get(expr.expr));
    vm.advance();
  }

  void execute(const program&, const statement::function&) noexcept {}

  void execute(const program& prog,
               const statement::if_else& if_else) noexcept {
    const auto condition =
        evaluate(prog, prog.expressions.get(if_else.condition));
    if (condition.as<bool>()) {
      execute(prog, prog.statements.get(if_else.then_block), true);
    } else {
      execute(prog, prog.statements.get(if_else.else_block), true);
    }
  }

  void execute(const program& prog,
               const statement::print_s& print_s) noexcept {
    out << to_string(evaluate(prog, prog.expressions.get(print_s.value)))
        << "\n";
    vm.advance();
  }

  void execute(const program&, const statement::return_s&) noexcept {}

  void execute(const program& prog,
               const statement::variable_s& variable_s) noexcept {
    vm.define_value(
        evaluate(prog, prog.expressions.get(variable_s.initializer)));
    vm.advance();
  }

  void execute(const program& prog,
               const statement::while_s& while_s) noexcept {
    const auto condition =
        evaluate(prog, prog.expressions.get(while_s.condition));
    if (condition.as<bool>()) {
      execute(prog, prog.statements.get(while_s.body), false);
    } else {
      vm.advance();
    }
  }

  value evaluate(const program& prog, const expression& expr) noexcept {
    return expr.storage.visit([&](const auto& e) { return evaluate(prog, e); });
  }

  value evaluate(const program& prog,
                 const expression::assignment& assignment) noexcept {
    const auto variable = prog.expressions.get(assignment.variable);
    Ensure(variable.storage.is_type<expression::variable>());
    const auto& v = variable.storage.as<expression::variable>();
    const auto info = v.info;
    const auto value = evaluate(prog, prog.expressions.get(assignment.value));
    vm.assign(info, value);
    return value;
  }

  value evaluate(const program& prog, const expression::binary& binary) {
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
        throw runtime_error("");
    }
  }

  value evaluate(const program&, const expression::call&) noexcept {
    return {};
  }

  value evaluate(const program&, const expression::group&) noexcept {
    return {};
  }

  value evaluate(const program& prog,
                 const expression::literal& literal) noexcept {
    return literal.storage.visit(overloaded{
        [](null) { return value{}; },
        [&](bool b) { return value{b}; },
        [&](double_id d) { return value{prog.double_literals.get(d)}; },
        [&](string_id) {
          return value{}; /* todo*/
        },
    });
  }

  value evaluate(const program&, const expression::unary&) noexcept {
    return {};
  }

  value evaluate(const program&,
                 const expression::variable& variable) noexcept {
    return vm.get(variable.info);
  }

 private:
  virtual_machine vm;
  std::ostream& out;
};

}  // namespace lox

#endif
