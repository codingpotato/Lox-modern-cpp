#include "to_string.h"

namespace lox {

static string indent_of_level(int level) noexcept {
  string result;
  for (auto i = 0; i < level; ++i) {
    result += "  ";
  }
  return result;
}

string to_string(const program& prog) noexcept {
  return to_string(prog, prog.statements.get(prog.start_block), 0);
}

string to_string(const program& prog, const statement& stat,
                 int level) noexcept {
  return stat.storage.visit(overloaded{[&prog, level](const auto& element) {
    return to_string(prog, element, level);
  }});
}

string to_string(const program& prog, const statement::block& block,
                 int level) noexcept {
  string result;
  result += indent_of_level(level) + "{\n";
  for (auto index = block.first; index < block.last; ++index) {
    result += to_string(prog, prog.statements.get(index), level + 1);
  }
  result += indent_of_level(level) + "}\n";
  return result;
}

string to_string(const program& prog, const statement::expression_s& expr,
                 int level) noexcept {
  return indent_of_level(level) +
         to_string(prog, prog.expressions.get(expr.expr)) + ";\n";
}

string to_string(const program&, const statement::function&, int) noexcept {
  return "";
}

string to_string(const program& prog, const statement::if_else& if_else,
                 int level) noexcept {
  return indent_of_level(level) + "if (" +
         to_string(prog, prog.expressions.get(if_else.condition)) + ")\n" +
         to_string(prog, prog.statements.get(if_else.then_block), level + 1) +
         indent_of_level(level) +
         (if_else.else_block == statement_id{}
              ? "\n"
              : "else\n" + to_string(prog,
                                     prog.statements.get(if_else.else_block),
                                     level + 1));
}

string to_string(const program& prog, const statement::print& print,
                 int level) noexcept {
  return indent_of_level(level) + "print " +
         to_string(prog, prog.expressions.get(print.value)) + ";\n";
}

string to_string(const program&, const statement::return_s&, int) noexcept {
  return "";
}

string to_string(const program& prog, const statement::variable_s& variable,
                 int level) noexcept {
  string initializer;
  if (variable.initializer != expression_id{}) {
    initializer =
        " = " + to_string(prog, prog.expressions.get(variable.initializer));
  }
  return indent_of_level(level) + "var " +
         prog.string_literals.get(variable.name) + initializer + ";\n";
}

string to_string(const program& prog, const statement::while_s& while_s,
                 int level) noexcept {
  return indent_of_level(level) + "while (" +
         to_string(prog, prog.expressions.get(while_s.condition)) + ")\n" +
         to_string(prog, prog.statements.get(while_s.body), level + 1);
}

string to_string(const program& prog, const expression& expr) noexcept {
  return expr.storage.visit(overloaded{
      [&prog](const auto& element) { return to_string(prog, element); }});
}

string to_string(expression::operator_t oper) {
  switch (oper) {
    case expression::operator_t::plus:
      return " + ";
    case expression::operator_t::minus:
      return " - ";
    case expression::operator_t::multiple:
      return " * ";
    case expression::operator_t::divide:
      return " / ";
    case expression::operator_t::equal:
      return " == ";
    case expression::operator_t::not_equal:
      return " != ";
    case expression::operator_t::great:
      return " > ";
    case expression::operator_t::less:
      return " < ";
    case expression::operator_t::great_equal:
      return " >= ";
    case expression::operator_t::less_equal:
      return " <= ";
    case expression::operator_t::logic_or:
      return " || ";
    case expression::operator_t::logic_and:
      return " && ";
  }
  throw internal_error{"Unknow operator."};
}

string to_string(const program& prog,
                 const expression::assignment& assignment) noexcept {
  return to_string(prog, prog.expressions.get(assignment.variable)) + " = " +
         to_string(prog, prog.expressions.get(assignment.value));
}

string to_string(const program& prog,
                 const expression::binary& binary) noexcept {
  return to_string(prog, prog.expressions.get(binary.left)) +
         to_string(binary.oper) +
         to_string(prog, prog.expressions.get(binary.right));
}

string to_string(const program&, const expression::call&) noexcept {
  return "";
}

string to_string(const program& prog, const expression::group& group) noexcept {
  return "(" + to_string(prog, prog.expressions.get(group.expr)) + ")";
}

string to_string(const program& prog,
                 const expression::literal& literal) noexcept {
  return literal.storage.visit(overloaded{
      [](null) { return string{"null"}; },
      [](bool b) { return b ? string{"true"} : string{"false"}; },
      [](int i) { return std::to_string(i); },
      [&prog](double_id id) {
        return std::to_string(prog.double_literals.get(id));
      },
      [&prog](string_id id) { return prog.string_literals.get(id); },
  });
}

string to_string(const program& prog, const expression::unary& unary) noexcept {
  return to_string(unary.oper) +
         to_string(prog, prog.expressions.get(unary.expr));
}

string to_string(const program&,
                 const expression::variable& variable) noexcept {
  return "v_" + std::to_string(variable.info.distance) + "_" +
         std::to_string(variable.info.index);
}

}  // namespace lox
