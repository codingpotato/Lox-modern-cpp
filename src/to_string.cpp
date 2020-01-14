#include "to_string.h"

namespace lox {

string to_string(const program& prog) noexcept {
  return to_string(prog, prog.statements.get(prog.start_block));
}

string to_string(const program& prog, const statement& stat) noexcept {
  return stat.visit(overloaded{
      [&prog](const auto& element) { return to_string(prog, element); }});
}

string to_string(const program& prog, const statement::block& block) noexcept {
  string result;
  for (auto index = block.first; index < block.last; ++index) {
    result += to_string(prog, prog.statements.get(index));
  }
  return result;
}

string to_string(const program& prog,
                 const statement::expression_s& expr) noexcept {
  return "(" + to_string(prog, prog.expressions.get(expr.expr)) + ")";
}

string to_string(const program&, const statement::for_s&) noexcept {
  return "";
}

string to_string(const program&, const statement::function&) noexcept {
  return "";
}

string to_string(const program&, const statement::if_else&) noexcept {
  return "";
}

string to_string(const program&, const statement::return_s&) noexcept {
  return "";
}

string to_string(const program&, const statement::variable_s&) noexcept {
  return "";
}

string to_string(const program&, const statement::while_s&) noexcept {
  return "";
}

string to_string(const program& prog, const expression& expr) noexcept {
  return expr.visit(overloaded{
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

string to_string(const program&, const expression::assignment&) noexcept {
  return "";
}

string to_string(const program& prog,
                 const expression::binary& binary) noexcept {
  return "(" + to_string(prog, prog.expressions.get(binary.left)) +
         to_string(binary.oper) +
         to_string(prog, prog.expressions.get(binary.right)) + ")";
}

string to_string(const program&, const expression::call&) noexcept {
  return "";
}

string to_string(const program&, const expression::group&) noexcept {
  return "";
}

string to_string(const program& prog,
                 const expression::literal& literal) noexcept {
  return std::visit(
      overloaded{
          [](int i) { return std::to_string(i); },
          [&prog](double_id id) {
            return std::to_string(prog.double_literals.get(id));
          },
          [&prog](string_id id) { return prog.string_literals.get(id); },
      },
      literal.value);
}

string to_string(const program&, const expression::unary&) noexcept {
  return "";
}

string to_string(const program&, const expression::variable&) noexcept {
  return "";
}

}  // namespace lox
