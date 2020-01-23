#ifndef LOX_TO_STRING_H
#define LOX_TO_STRING_H

#include "expression.h"
#include "program.h"
#include "statement.h"
#include "types.h"

namespace lox {

string to_string(const program& prog) noexcept;

string to_string(const program& prog, const statement& stat,
                 int level) noexcept;

string to_string(const program& prog, const statement::block& block,
                 int level) noexcept;
string to_string(const program&, const statement::expression_s&, int) noexcept;
string to_string(const program&, const statement::function&, int) noexcept;
string to_string(const program&, const statement::if_else&, int) noexcept;
string to_string(const program&, const statement::print_s&, int) noexcept;
string to_string(const program&, const statement::return_s&, int) noexcept;
string to_string(const program&, const statement::variable_s&, int) noexcept;
string to_string(const program&, const statement::while_s&, int) noexcept;

string to_string(const program& prog, const expression& expr) noexcept;

string to_string(const program&, const expression::assignment&) noexcept;
string to_string(const program&, const expression::binary&) noexcept;
string to_string(const program&, const expression::call&) noexcept;
string to_string(const program&, const expression::group&) noexcept;
string to_string(const program&, const expression::literal&) noexcept;
string to_string(const program&, const expression::unary&) noexcept;
string to_string(const program&, const expression::variable&) noexcept;

}  // namespace lox

#endif
