#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "exception.h"
#include "expression.h"
#include "program.h"
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
      prog.statements.add(std::in_place_type<statement::block>,
                          parse_block(prog));
      prog.start_block = prog.statements.size() - 1;
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
    const auto first = prog.statements.size();
    prog.statements.add(block_statements.cbegin(), block_statements.cend());
    const auto last = prog.statements.size();
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
    const auto name_id = prog.string_literals.add(
        consume(token::l_identifier, "Expect function name.").lexeme);
    const auto first_parameter = prog.string_literals.size();
    consume(token::left_paren, "Expect '(' after function name.");
    if (!check(token::right_paren)) {
      do {
        prog.string_literals.add(
            consume(token::l_identifier,
                    "Expect parameter name for function '" +
                        string{prog.string_literals.get(name_id)} + "'.")
                .lexeme);
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after parameters.");
    consume(token::left_brace, "Expect '{' before function body.");
    const auto last_parameter = prog.string_literals.size();
    block_statements.emplace_back(std::in_place_type<statement::block>,
                                  parse_block(prog));
    block_statements.emplace_back(std::in_place_type<statement::function>,
                                  name_id, first_parameter, last_parameter);
  }

  // varDecl: "var" IDENTIFIER ( "=" expression ) ";"
  void parse_variable_declaration(program &prog,
                                  statement_vector &block_statements) {
    const auto name_id = prog.string_literals.add(
        consume(token::l_identifier, "Expect variable name").lexeme);
    consume(token::equal, "Expect initializer");
    const auto expr = parse_expression(prog);
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

  // forStmt: "for" "(" ( varDecl | exprStmt | ";" ) expression ? ";"
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
    expression_id condition{};
    if (!check(token::semicolon)) {
      condition = parse_expression(prog);
    } else {
      condition = prog.expressions.add(true);
    }
    consume(token::semicolon, "Expect ';' after loop condition.");
    expression_id increament{};
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
    expression_id value{};
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
  expression_id parse_expression(program &prog) {
    return parse_assignment(prog);
  }

  // assignment is an expression!
  // assignment: IDENTIFIER "=" assignment | logic_or
  expression_id parse_assignment(program &prog) {
    const auto index = parse_or(prog);
    if (match(token::equal)) {
      const auto value = parse_assignment(prog);
      if (const auto &expr = prog.expressions.get(index);
          expr.storage.is_type<expression::variable>()) {
        return prog.expressions.add(
            std::in_place_type<expression::assignment>,
            expr.storage.get<expression::variable>().name, value);
      }
      throw parse_error{"Invalid assignment target."};
    }
    return index;
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
  //           IDENTIFIER
  expression_id parse_primary(program &prog) {
    if (match(token::k_false)) {
      return prog.expressions.add(false);
    }
    if (match(token::k_true)) {
      return prog.expressions.add(std::in_place_type<expression::literal>,
                                  true);
    }
    if (match(token::k_nil)) {
      return prog.expressions.add(std::in_place_type<expression::literal>);
    }
    if (match(token::l_number)) {
      const auto &literal = previous().value;
      if (std::holds_alternative<int>(literal.value)) {
        return prog.expressions.add(std::in_place_type<expression::literal>,
                                    std::get<int>(literal.value));
      }
      if (std::holds_alternative<double>(literal.value)) {
        return prog.expressions.add(
            std::in_place_type<expression::literal>,
            prog.double_literals.add(std::get<double>(literal.value)));
      }
    }
    if (match(token::l_string)) {
      return prog.expressions.add(
          std::in_place_type<expression::literal>,
          prog.string_literals.add(std::get<string>(previous().value.value)));
    }
    if (match(token::l_identifier)) {
      return prog.expressions.add(std::in_place_type<expression::variable>,
                                  prog.string_literals.add(previous().lexeme));
    }
    if (match(token::left_paren)) {
      const auto expr = parse_expression(prog);
      consume(token::right_paren, "Expect ')' after expression.");
      return prog.expressions.add(std::in_place_type<expression::group>, expr);
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

  const token &consume(token::type_t t, const string &message) {
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
