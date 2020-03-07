#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <optional>

#include "exception.h"
#include "expression.h"
#include "program.h"
#include "resolver.h"
#include "statement.h"
#include "token.h"
#include "types.h"

namespace lox {

class parser {
 public:
  program parse(token_vector ts) {
    try {
      tokens = std::move(ts);
      current = tokens.cbegin();
      program prog;
      prog.start_block = prog.statements.add(parse_block(prog));
      return prog;
    } catch (const std::exception &e) {
      synchronize();
      throw;
    }
  }

  // statement: forStmt | ifStmt | printStmt | returnStmt | whileStmt | block |
  // exprStmt
  statement parse_statement(
      program &prog, std::optional<statement> post_statement_for_block = {}) {
    if (match(token::k_for)) {
      return parse_for(prog);
    } else if (match(token::k_if)) {
      return parse_if_else(prog);
    } else if (match(token::k_print)) {
      return parse_print(prog);
    } else if (match(token::k_return)) {
      return parse_return(prog);
    } else if (match(token::k_while)) {
      return parse_while(prog);
    } else if (match(token::left_brace)) {
      return parse_block(prog, post_statement_for_block);
    } else {
      return parse_expression_s(prog);
    }
  }

  // block: "{" declaration* "}"
  statement parse_block(program &prog,
                        std::optional<statement> post_stat_for_block = {}) {
    if (!post_stat_for_block) {
      resolver.begin_scope();
    }

    statement_vector block_statements;
    while (!is_end() && !check(token::right_brace)) {
      block_statements.emplace_back(parse_declaration(prog));
    }
    if (post_stat_for_block) {
      block_statements.push_back(*post_stat_for_block);
    }
    if (!is_end()) {
      consume(token::right_brace, "Expect '}' after block.");
    }
    const auto first = prog.statements.size();
    prog.statements.add(block_statements.cbegin(), block_statements.cend());
    const auto last = prog.statements.size();

    if (!post_stat_for_block) {
      resolver.end_scope();
    }
    return {std::in_place_type<statement::block>, first, last};
  }

  // declaration: function declaration | variable declaration | statement
  statement parse_declaration(program &prog) {
    if (match(token::k_fun)) {
      return parse_function(prog);
    } else if (match(token::k_var)) {
      return parse_variable_declaration(prog);
    } else {
      return parse_statement(prog);
    }
  }

  // exprStmt: expression ";"
  statement parse_expression_s(program &prog) {
    const auto expr = parse_expression(prog);
    consume(token::semicolon, "Expect ';' after value.");
    return {std::in_place_type<statement::expression_s>, expr};
  }

  // function declaration: IDENTIFIER "(" parameters? ")" block
  // parameters: IDENTIFIER ( "," IDENTIFIER )*
  statement parse_function(program &prog) {
    const auto &name = consume(token::l_identifier, "Expect function name.")
                           .value.storage.as<string>();
    const auto name_id = prog.string_literals.add(name);

    resolver.declear(name);
    resolver.define(name);

    resolver.begin_scope();
    consume(token::left_paren, "Expect '(' after function name.");
    const auto first_parameter = prog.string_literals.size();
    if (!check(token::right_paren)) {
      do {
        const auto &parameter =
            consume(token::l_identifier,
                    "Expect parameter name for function '" +
                        string{prog.string_literals.get(name_id)} + "'.")
                .value.storage.as<string>();
        prog.string_literals.add(parameter);

        resolver.declear(parameter);
        resolver.define(parameter);
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after parameters.");

    consume(token::left_brace, "Expect '{' before function body.");
    const auto last_parameter = prog.string_literals.size();
    const auto body = prog.statements.add(parse_block(prog));
    resolver.end_scope();
    return {std::in_place_type<statement::function>, name_id, first_parameter,
            last_parameter, body};
  }

  // forStmt: "for" "(" ( varDecl | exprStmt | ";" ) expression ? ";"
  //           expression ? ")" statement
  statement parse_for(program &prog) {
    resolver.begin_scope();
    consume(token::left_paren, "Expect '(' after 'for'.");
    std::optional<statement> initializer;
    if (match(token::k_var)) {
      initializer = parse_variable_declaration(prog);
    } else if (!match(token::semicolon)) {
      initializer = parse_expression_s(prog);
    }
    expression_id condition;
    if (!check(token::semicolon)) {
      condition = parse_expression(prog);
    } else {
      condition = prog.expressions.add(true);
    }
    consume(token::semicolon, "Expect ';' after loop condition.");

    resolver.begin_scope();
    std::optional<statement> increament;
    if (!check(token::right_paren)) {
      const auto expr = parse_expression(prog);
      increament.emplace(std::in_place_type<statement::expression_s>, expr);
    }
    consume(token::right_paren, "Expect ')' after for clauses.");

    const auto body = prog.statements.add(parse_statement(prog, increament));
    statement while_stat{std::in_place_type<statement::while_s>, condition,
                         body};
    resolver.end_scope();
    auto first = prog.statements.size();
    if (initializer) {
      prog.statements.add(*initializer);
    }
    const auto last = prog.statements.add(while_stat);
    resolver.end_scope();
    return {std::in_place_type<statement::block>, first, last + 1};
  }

  // ifStmt: "if" "(" expression ")" statement ( "else" statement )?
  statement parse_if_else(program &prog) {
    consume(token::left_paren, "Expect '(' after 'if'.");
    const auto condition = parse_expression(prog);
    consume(token::right_paren, "Expect ')' after if condition.");
    const auto then_block = prog.statements.add(parse_statement(prog));
    auto else_block = statement_id{};
    if (match(token::k_else)) {
      else_block = prog.statements.add(parse_statement(prog));
    }
    return {std::in_place_type<statement::if_else>, condition, then_block,
            else_block};
  }

  // printStmt: "print" expression ";"
  statement parse_print(program &prog) {
    expression_id value = parse_expression(prog);
    consume(token::semicolon, "Expect ';' after print value.");
    return {std::in_place_type<statement::print_s>, value};
  }

  // returnStmt: "return" expression? ";"
  statement parse_return(program &prog) {
    expression_id value{};
    if (!check(token::semicolon)) {
      value = parse_expression(prog);
    }
    consume(token::semicolon, "Expect ';' after return value.");
    return {std::in_place_type<statement::return_s>, value};
  }

  // varDecl: "var" IDENTIFIER ( "=" expression ) ";"
  statement parse_variable_declaration(program &prog) {
    const auto &name = consume(token::l_identifier, "Expect variable name")
                           .value.storage.as<string>();
    if (resolver.is_in_variable_declearation(name)) {
      throw parse_error_object(
          "Cannot read local variable in its own initializer.");
    }
    resolver.declear(name);
    const auto name_id = prog.string_literals.add(name);

    expression_id initializer;
    if (match(token::equal)) {
      initializer = parse_expression(prog);
    }
    consume(token::semicolon, "Expect ';' after value.");
    resolver.define(name);
    return {std::in_place_type<statement::variable_s>, name_id, initializer};
  }

  // whileStmt: "while" "(" expression ")" statement
  statement parse_while(program &prog) {
    consume(token::left_paren, "Expect '(' after 'while'.");
    const auto condition = parse_expression(prog);
    consume(token::right_paren, "Expect ')' after if condition.");
    const auto body = prog.statements.add(parse_statement(prog));
    return {std::in_place_type<statement::while_s>, condition, body};
  }

  // expression: assignment
  expression_id parse_expression(program &prog) {
    return parse_assignment(prog);
  }

  // assignment is an expression!
  // assignment: IDENTIFIER "=" assignment | logic_or
  expression_id parse_assignment(program &prog) {
    const auto expr_id = parse_or(prog);
    if (match(token::equal)) {
      const auto value = parse_assignment(prog);
      if (const auto &expr = prog.expressions.get(expr_id);
          expr.storage.is_type<expression::variable>()) {
        return prog.expressions.add(std::in_place_type<expression::assignment>,
                                    expr_id, value);
      }
      throw parse_error_object("Invalid assignment target.");
    }
    return expr_id;
  }

  template <typename Fun>
  expression_id parse_binary(program &prog,
                             const std::initializer_list<token::type_t> &types,
                             Fun parse_sub_expression) noexcept {
    auto left = parse_sub_expression(prog);
    while (match(types)) {
      const auto op = expression::from_token_type(previous().type);
      const auto right = parse_sub_expression(prog);
      left = prog.expressions.add(std::in_place_type<expression::binary>, left,
                                  op, right);
    }
    return left;
  }

  // equality: comparison ( ( "!=" | "==" ) comparison )*
  expression_id parse_equality(program &prog) {
    return parse_binary(
        prog, {token::bang_equal, token::equal_equal},
        [this](program &prog) { return parse_comparison(prog); });
  }

  // comparison: addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
  expression_id parse_comparison(program &prog) {
    return parse_binary(
        prog,
        {token::greater, token::greater_equal, token::less, token::less_equal},
        [this](program &prog) { return parse_addition(prog); });
  }

  // addition: multiplication ( ( "-" | "+" ) multiplication )*
  expression_id parse_addition(program &prog) {
    return parse_binary(
        prog, {token::minus, token::plus},
        [this](program &prog) { return parse_multiplication(prog); });
  }

  // multiplication: unary ( ( "/" | "*" ) unary )*
  expression_id parse_multiplication(program &prog) {
    return parse_binary(prog, {token::slash, token::star},
                        [this](program &prog) { return parse_unary(prog); });
  }

  // unary: ( "!" | "-" ) unary | call
  expression_id parse_unary(program &prog) {
    if (match({token::bang, token::minus})) {
      const auto op = expression::from_token_type(previous().type);
      const auto expr = parse_unary(prog);
      return prog.expressions.add(std::in_place_type<expression::unary>, op,
                                  expr);
    }
    return parse_call(prog);
  }

  // call: primary ( "(" arguments? ")" )*
  expression_id parse_call(program &prog) {
    auto callee = parse_primary(prog);
    if (match(token::left_paren)) {
      callee = parse_arguments(prog, callee);
    }
    return callee;
  }

  // arguments: expression ( "," expression )*
  expression_id parse_arguments(program &prog, expression_id callee) {
    const auto first = prog.parameters.size();
    if (!check(token::right_paren)) {
      do {
        prog.parameters.add(parse_expression(prog));
      } while (match(token::comma));
    }
    const auto last = prog.parameters.size();
    const auto index = prog.expressions.add(
        std::in_place_type<expression::call>, callee, first, last);
    consume(token::right_paren, "Expect ')' after arguments.");
    return index;
  }

  // logic_or: logic_and ( "or" logic_and )*
  expression_id parse_or(program &prog) {
    return parse_binary(prog, {token::k_or},
                        [this](program &prog) { return parse_and(prog); });
  }

  // logic_and: equality ( "and" equality )*
  expression_id parse_and(program &prog) {
    return parse_binary(prog, {token::k_and},
                        [this](program &prog) { return parse_equality(prog); });
  }

  // primary: NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")" |
  // IDENTIFIER
  expression_id parse_primary(program &prog) {
    if (match(token::k_false)) {
      return prog.expressions.add(false);
    }
    if (match(token::k_true)) {
      return prog.expressions.add(true);
    }
    if (match(token::k_nil)) {
      return prog.expressions.add(std::in_place_type<expression::literal>);
    }
    if (match(token::l_number)) {
      const auto &literal = previous().value;
      return prog.expressions.add(
          std::in_place_type<expression::literal>,
          prog.double_literals.add(literal.storage.as<double>()));
    }
    if (match(token::l_string)) {
      return prog.expressions.add(
          std::in_place_type<expression::literal>,
          prog.string_literals.add(previous().value.storage.as<string>()));
    }
    if (match(token::l_identifier)) {
      const auto &name = previous().value.storage.as<string>();
      return prog.expressions.add(std::in_place_type<expression::variable>,
                                  resolver.resolve(name));
    }
    if (match(token::left_paren)) {
      const auto expr = parse_expression(prog);
      consume(token::right_paren, "Expect ')' after expression.");
      return prog.expressions.add(std::in_place_type<expression::group>, expr);
    }
    throw parse_error_object("Expect primary.");
  }

  bool match(const std::initializer_list<token::type_t> &types) noexcept {
    for (auto t : types) {
      if (match(t)) {
        return true;
      }
    }
    return false;
  }

  bool match(token::type_t t) noexcept {
    if (check(t)) {
      advance();
      return true;
    }
    return false;
  }

  bool check(token::type_t type) const noexcept {
    if (is_end()) {
      return false;
    }
    return peek().type == type;
  }

  const token &advance() noexcept {
    if (!is_end()) {
      ++current;
    }
    return previous();
  }

  const token &consume(token::type_t t, const string &message) {
    if (check(t)) {
      return advance();
    }
    throw parse_error_object(message);
  }

  void synchronize() {
    advance();
    while (!is_end()) {
      if (previous().type != token::semicolon) return;
      switch (peek().type) {
        case token::k_class:
        case token::k_fun:
        case token::k_var:
        case token::k_for:
        case token::k_if:
        case token::k_while:
        case token::k_return:
          return;
        default:
          throw parse_error_object("Unknow keyword.");
      }
      advance();
    }
  }

  const token &peek() const noexcept { return *current; }
  const token &previous() const noexcept { return *(current - 1); }

  bool is_end() const noexcept { return peek().type == token::eof; }

  parse_error parse_error_object(const string &message) const {
    return parse_error{"[" + to_string(*current) + "]: " + message};
  }

  token_vector tokens;
  token_vector::const_iterator current;
  resolver_t resolver;
};

}  // namespace lox

#endif
