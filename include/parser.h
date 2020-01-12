#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <vector>

#include "exception.h"
#include "expression.h"
#include "program.h"
#include "statement.h"
#include "token.h"

namespace lox {

class parser {
 public:
  program parse(token_vector ts) {
    try {
      tokens = std::move(ts);
      current = tokens.cbegin();
      program prog;
      prog.add<statement>(std::in_place_type<statement::block>,
                          parse_block(prog));
      prog.start_block = prog.size<statement>() - 1;
      return prog;
    } catch (const std::exception &e) {
      synchronize();
      throw;
    }
  }

  // block: "{" declaration* "}"
  statement::block parse_block(program &prog) {
    statement_vector block_statements;
    while (!is_end() && !check(token::right_brace)) {
      parse_declaration(prog, block_statements);
    }
    if (!is_end()) {
      consume(token::right_brace, "Expect '}' after block.");
    }
    auto first = static_cast<index_t>(prog.size<statement>());
    std::copy(block_statements.cbegin(), block_statements.cend(),
              std::back_inserter(prog.statements));
    index_t last = static_cast<index_t>(prog.size<statement>());
    return {first, last};
  }

  // declaration: function declaration | variable declaration | statement
  void parse_declaration(program &prog, statement_vector &block_statements) {
    if (match(token::k_fun)) {
      parse_function(prog, block_statements);
    } else if (match(token::k_var)) {
      parse_variable_declaration(prog, block_statements);
    } else {
      parse_statement(prog, block_statements);
    }
  }

  // function declaration: IDENTIFIER "(" parameters? ")" block
  // parameters: IDENTIFIER ( "," IDENTIFIER )*
  void parse_function(program &prog, statement_vector &block_statements) {
    const auto name_id = prog.strings.add(
        consume(token::identifier, "Expect function name.").lexeme);
    const auto first_parameter = prog.strings.size();
    consume(token::left_paren, "Expect '(' after function name.");
    if (!check(token::right_paren)) {
      do {
        prog.strings.add(consume(token::identifier,
                                 "Expect parameter name for function '" +
                                     std::string{prog.strings.get(name_id)} +
                                     "'.")
                             .lexeme);
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after parameters.");
    consume(token::left_brace, "Expect '{' before function body.");
    const auto last_parameter = prog.strings.size();
    block_statements.emplace_back(std::in_place_type<statement::block>,
                                  parse_block(prog));
    block_statements.emplace_back(std::in_place_type<statement::function>,
                                  name_id, first_parameter, last_parameter);
  }

  // varDecl: "var" IDENTIFIER ( "=" expression ) ";"
  void parse_variable_declaration(program &prog,
                                  statement_vector &block_statements) {
    const auto name_id = prog.strings.add(
        consume(token::identifier, "Expect variable name").lexeme);
    consume(token::equal, "Expect initializer");
    auto expr = parse_expression(prog);
    consume(token::semicolon, "Expect ';' after value.");
    block_statements.emplace_back(std::in_place_type<statement::variable_s>,
                                  name_id, expr);
  }

  // statement: forStmt | ifStmt | returnStmt | whileStmt | block | exprStmt
  void parse_statement(program &prog, statement_vector &block_statements) {
    if (match(token::k_for)) {
      parse_for(prog, block_statements);
    } else if (match(token::k_if)) {
      parse_if_else(prog, block_statements);
    } else if (match(token::k_return)) {
      parse_return(prog, block_statements);
    } else if (match(token::k_while)) {
      parse_while(prog, block_statements);
    } else if (match(token::left_brace)) {
      block_statements.emplace_back(std::in_place_type<statement::block>,
                                    parse_block(prog));
    } else {
      parse_expr_statement(prog, block_statements);
    }
  }

  // forStmt → "for" "(" ( varDecl | exprStmt | ";" ) expression ? ";"
  //           expression ? ")" statement
  void parse_for(program &prog, statement_vector &block_statements) {
    consume(token::left_paren, "Expect '(' after 'for'.");
    auto has_initializer = false;
    if (match(token::k_var)) {
      parse_variable_declaration(prog, block_statements);
      has_initializer = true;
    } else if (!match(token::semicolon)) {
      parse_expr_statement(prog, block_statements);
      has_initializer = true;
    }
    index_t condition = invalid_index;
    if (!check(token::semicolon)) {
      condition = parse_expression(prog);
    } else {
      condition =
          prog.add<expression>(prog.size<expression>(),
                               std::in_place_type<expression::literal>, true);
    }
    consume(token::semicolon, "Expect ';' after loop condition.");
    index_t increament = invalid_index;
    if (!check(token::right_paren)) {
      increament = parse_expression(prog);
    }
    consume(token::right_paren, "Expect ')' after for clauses.");

    block_statements.emplace_back(std::in_place_type<statement::block>,
                                  parse_block(prog));
    block_statements.emplace_back(std::in_place_type<statement::for_s>,
                                  has_initializer, condition, increament);
  }

  // ifStmt: "if" "(" expression ")" statement ( "else" statement )?
  void parse_if_else(program &prog, statement_vector &block_statements) {
    consume(token::left_paren, "Expect '(' after 'if'.");
    const auto condition = parse_expression(prog);
    consume(token::right_paren, "Expect ')' after if condition.");
    parse_statement(prog, block_statements);
    if (match(token::k_else)) {
      parse_statement(prog, block_statements);
    }
    block_statements.emplace_back(std::in_place_type<statement::if_else>,
                                  condition);
  }

  // returnStmt: "return" expression? ";"
  void parse_return(program &prog, statement_vector &block_statements) {
    index_t value = invalid_index;
    if (!check(token::semicolon)) {
      value = parse_expression(prog);
    }
    consume(token::semicolon, "Expect ';' after return value.");
    block_statements.emplace_back(std::in_place_type<statement::return_s>,
                                  value);
  }

  // whileStmt: "while" "(" expression ")" statement
  void parse_while(program &prog, statement_vector &block_statements) {
    consume(token::left_paren, "Expect '(' after 'while'.");
    const auto condition = parse_expression(prog);
    consume(token::right_paren, "Expect ')' after if condition.");
    block_statements.emplace_back(std::in_place_type<statement::block>,
                                  parse_block(prog));
    block_statements.emplace_back(std::in_place_type<statement::while_s>,
                                  condition);
  }

  // exprStmt: expression ";"
  void parse_expr_statement(program &prog, statement_vector &block_statements) {
    const auto expr = parse_expression(prog);
    consume(token::semicolon, "Expect ';' after value.");
    block_statements.emplace_back(std::in_place_type<statement::expression_s>,
                                  expr);
  }

  // expression: assignment
  index_t parse_expression(program &prog) { return parse_assignment(prog); }

  // assignment is an expression!
  // assignment: IDENTIFIER "=" assignment | logic_or
  index_t parse_assignment(program &prog) {
    const auto first = prog.size<expression>();
    const auto index = parse_or(prog);
    if (match(token::equal)) {
      parse_assignment(prog);
      if (const auto &expr = prog.get<expression>(index);
          expr.is_type<expression::variable>()) {
        return prog.add<expression>(first,
                                    std::in_place_type<expression::assignment>,
                                    expr.get<expression::variable>().name);
      }
      throw parse_error{"Invalid assignment target."};
    }
    return index;
  }

  template <typename Fun>
  index_t parse_binary(program &prog,
                       const std::initializer_list<token::type_t> &types,
                       Fun parse_sub_expression) noexcept {
    const auto first = prog.size<expression>();
    auto left = parse_sub_expression(prog);
    while (match(types)) {
      const auto op = expression::from_token_type(previous().type);
      parse_sub_expression(prog);
      left = prog.add<expression>(first, std::in_place_type<expression::binary>,
                                  op);
    }
    return left;
  }

  // equality → comparison ( ( "!=" | "==" ) comparison )*
  index_t parse_equality(program &prog) {
    return parse_binary(
        prog, {token::bang_equal, token::equal_equal},
        [this](program &prog) { return parse_comparison(prog); });
  }

  // comparison → addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
  index_t parse_comparison(program &prog) {
    return parse_binary(
        prog,
        {token::greater, token::greater_equal, token::less, token::less_equal},
        [this](program &prog) { return parse_addition(prog); });
  }

  // addition → multiplication ( ( "-" | "+" ) multiplication )*
  index_t parse_addition(program &prog) {
    return parse_binary(
        prog, {token::minus, token::plus},
        [this](program &prog) { return parse_multiplication(prog); });
  }

  // multiplication → unary ( ( "/" | "*" ) unary )*
  index_t parse_multiplication(program &prog) {
    return parse_binary(prog, {token::slash, token::star},
                        [this](program &prog) { return parse_unary(prog); });
  }

  // unary → ( "!" | "-" ) unary | call
  index_t parse_unary(program &prog) {
    const auto first = prog.size<expression>();
    if (match({token::bang, token::minus})) {
      const auto op = expression::from_token_type(previous().type);
      parse_unary(prog);
      return prog.add<expression>(first, std::in_place_type<expression::unary>,
                                  op);
    }
    return parse_call(prog);
  }

  // call → primary ( "(" arguments? ")" )*
  index_t parse_call(program &prog) {
    auto callee = parse_primary(prog);
    if (match(token::left_paren)) {
      callee = parse_arguments(prog, callee);
    }
    return callee;
  }

  // arguments → expression ( "," expression )*
  index_t parse_arguments(program &prog, index_t first) {
    if (!check(token::right_paren)) {
      do {
        parse_expression(prog);
      } while (match(token::comma));
    }
    const auto index =
        prog.add<expression>(first, std::in_place_type<expression::call>);
    consume(token::right_paren, "Expect ')' after arguments.");
    return index;
  }

  // logic_or → logic_and ( "or" logic_and )*
  int parse_or(program &prog) {
    return parse_binary(prog, {token::k_or},
                        [this](program &prog) { return parse_and(prog); });
  }

  // logic_and → equality ( "and" equality )*
  int parse_and(program &prog) {
    return parse_binary(prog, {token::k_and},
                        [this](program &prog) { return parse_equality(prog); });
  }

  // primary → NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")" |
  //           IDENTIFIER
  index_t parse_primary(program &prog) {
    const auto first = prog.size<expression>();
    if (match(token::k_false)) {
      return prog.add<expression>(
          first, std::in_place_type<expression::literal>, false);
    }
    if (match(token::k_true)) {
      return prog.add<expression>(
          first, std::in_place_type<expression::literal>, true);
    }
    if (match(token::k_nil)) {
      return prog.add<expression>(first,
                                  std::in_place_type<expression::literal>);
    }
    if (match(token::number)) {
      const auto &literal = previous().value;
      if (std::holds_alternative<int>(literal.value)) {
        return prog.add<expression>(first,
                                    std::in_place_type<expression::literal>,
                                    std::get<int>(literal.value));
      }
      if (std::holds_alternative<double>(literal.value)) {
        return prog.add<expression>(first,
                                    std::in_place_type<expression::literal>,
                                    std::get<double>(literal.value));
      }
    }
    if (match(token::string)) {
      return prog.add<expression>(
          first, std::in_place_type<expression::literal>,
          prog.strings.add(std::get<std::string>(previous().value.value)));
    }
    if (match(token::identifier)) {
      return prog.add<expression>(first,
                                  std::in_place_type<expression::variable>,
                                  prog.strings.add(previous().lexeme));
    }
    if (match(token::left_paren)) {
      parse_expression(prog);
      consume(token::right_paren, "Expect ')' after expression.");
      return prog.add<expression>(first, std::in_place_type<expression::group>);
    }
    throw parse_error{"Expect expression."};
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

  const token &consume(token::type_t t, const std::string &message) {
    if (check(t)) return advance();
    throw parse_error{message};
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
          throw parse_error{""};
      }
      advance();
    }
  }

  const token &peek() const noexcept { return *current; }
  const token &previous() const noexcept { return *(current - 1); }

  bool is_end() const noexcept { return peek().type == token::eof; }

  token_vector tokens;
  token_vector::const_iterator current;
};

}  // namespace lox

#endif
