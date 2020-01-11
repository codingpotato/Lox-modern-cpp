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
  explicit parser(token_vector tokens) noexcept : tokens_{std::move(tokens)} {}

  /*program parse() {
    program prog;
    while (!is_end()) {
      parse_declaration(prog);
    }
    return prog;
  }*/

 private:
  // declaration → funDecl | varDecl | statement
  /*void parse_declaration(program &prog) {
    try {
      if (match(token::k_fun)) {
        parse_function(prog);
      } else if (match(token::k_var)) {
        parse_var_decl(prog);
      } else {
        parse_statement(prog);
      }
    } catch (const parse_error &e) {
      synchronize();
      throw;
    }
  }

  // function → IDENTIFIER "(" parameters? ")" block
  // parameters → IDENTIFIER ( "," IDENTIFIER )*
  void parse_function(program &prog) {
    auto name = consume(token::identifier, "Expect function name.");
    std::vector<string> parameters;
    consume(token::left_paren, "Expect '(' after function name.");
    if (!check(token::right_paren)) {
      do {
        if (parameters.size() >= 8) {
          throw runtime_error{"Cannot have more than 8 arguments."};
        }
        auto parameter =
            consume(token::identifier, "Expect parameter name for function '" +
                                           name.get_lexeme() + "'.");
        parameters.emplace_back(parameter.get_lexeme());
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after parameters.");
    consume(token::left_brace, "Expect '{' before function body.");
    statements block;
    parse_block(block);
    if (block.size() == 1 && block[0].is_type<statement::block>()) {
      ss.emplace_back(std::in_place_type<statement::function>, name.lexeme,
                      parameters, block[0].get<statement::block>().statements_);
    } else {
      throw runtime_error{"Expect function block."};
    }
  }

  // varDecl → "var" IDENTIFIER ( "=" expression ) ";"
  void parse_var_decl(program &prog) {
    auto name = consume(token::identifier, "Expect variable name");
    consume(token::equal, "Expect initializer");
    auto expr = parse_expression();
    consume(token::semicolon, "Expect ';' after value.");
    prog.statements.emplace_back(std::in_place_type<statement::var>,
                                 name.lexeme, std::move(expr));
  }

  // statement → forStmt | ifStmt | returnStat | whileStmt | block | exprStmt
  void parse_statement(program &prog) {
    if (match(token::k_for)) {
      parse_for(prog);
    } else if (match(token::k_if)) {
      parse_if_else(prog);
    } else if (match(token::k_return)) {
      parse_return(prog);
    } else if (match(token::k_while)) {
      parse_while(prog);
    } else if (match(token::left_brace)) {
      parse_block(prog);
    } else {
      parse_expr_statement(prog);
    }
  }

  // forStmt → "for" "(" ( varDecl | exprStmt | ";" ) expression ? ";"
  //           expression ? ")" statement
  void parse_for(program &prog) {
    consume(token::left_paren, "Expect '(' after 'for'.");
    statement_indices initializer;
    if (match(token::k_var)) {
      parse_var_decl(initializer);
    } else if (!match(token::semicolon)) {
      parse_expr_statement(initializer);
    }
    expression condition;
    if (!check(token::semicolon)) {
      parse_expression(condition);
    }
    consume(token::semicolon, "Expect ';' after loop condition.");
    expression increament;
    if (!check(token::right_paren)) {
      parse_expression(increament);
    }
    consume(token::right_paren, "Expect ')' after for clauses.");

    statements body;
    parse_statement(body);
    statements &new_body =
        body.size() > 0 && body[0].is_type<statement::block>()
            ? body[0].get<statement::block>().statements_
            : body;

    if (!increament.empty()) {
      new_body.emplace_back(std::in_place_type<statement::expr>,
                            std::move(increament));
    }
    if (condition.empty()) {
      condition.add(std::in_place_type<expression::literal>, true);
    }
    if (initializer.empty()) {
      ss.emplace_back(std::in_place_type<statement::while_loop>,
                      std::move(condition), std::move(new_body));
    } else {
      initializer.emplace_back(std::in_place_type<statement::while_loop>,
                               std::move(condition), std::move(new_body));
      ss.emplace_back(std::in_place_type<statement::block>,
                      std::move(initializer));
    }
  }

  // ifStmt → "if" "(" expression ")" statement ( "else" statement )?
  void parse_if_else(statements &ss) {
    consume(token::left_paren, "Expect '(' after 'if'.");
    auto condition = parse_expression();
    consume(token::right_paren, "Expect ')' after if condition.");
    statements ts;
    parse_statement(ts);
    const statements &new_ts =
        ts.size() > 0 && ts[0].is_type<statement::block>()
            ? ts[0].get<statement::block>().statements_
            : ts;
    statements es;
    if (match(token::k_else)) {
      parse_statement(es);
    }
    const statements &new_es =
        es.size() > 0 && es[0].is_type<statement::block>()
            ? es[0].get<statement::block>().statements_
            : es;
    ss.emplace_back(std::in_place_type<statement::if_else>,
                    std::move(condition), std::move(new_ts), std::move(new_es));
  }

  // returnStmt → "return" expression? ";"
  void parse_return(statements &ss) {
    expression value;
    if (!check(token::semicolon)) {
      parse_expression(value);
    }
    consume(token::semicolon, "Expect ';' after return value.");
    ss.emplace_back(std::in_place_type<statement::return_s>, value);
  }

  // whileStmt → "while" "(" expression ")" statement
  void parse_while(statements &ss) {
    consume(token::left_paren, "Expect '(' after 'if'.");
    auto condition = parse_expression();
    consume(token::right_paren, "Expect ')' after if condition.");
    statements body;
    parse_statement(body);
    const statements &new_body =
        body.size() > 0 && body[0].is_type<statement::block>()
            ? body[0].get<statement::block>().statements_
            : body;
    ss.emplace_back(std::in_place_type<statement::while_loop>,
                    std::move(condition), std::move(new_body));
  }

  // block → "{" declaration* "}"
  void parse_block(statements &ss) {
    statements bs;
    while (!is_end() && !check(token::right_brace)) {
      parse_declaration(bs);
    }
    consume(token::right_brace, "Expect '}' after block.");
    ss.emplace_back(std::in_place_type<statement::block>, std::move(bs));
  }

  // exprStmt  → expression ";"
  void parse_expr_statement(statements &ss) {
    auto expr = parse_expression();
    consume(token::semicolon, "Expect ';' after value.");
    ss.emplace_back(std::in_place_type<statement::expr>, std::move(expr));
  }

  expression parse_expression() {
    expression expr;
    parse_expression(expr);
    return expr;
  }

  // expression → assignment
  int parse_expression(expression &expr) { return parse_assignment(expr); }

  // assignment → IDENTIFIER "=" assignment | logic_or
  int parse_assignment(expression &expr) {
    auto n = parse_or(expr);
    if (match(token::equal)) {
      auto v = parse_assignment(expr);
      if (expr[n].is_type<expression::variable>()) {
        return expr.add(std::in_place_type<expression::assignment>,
                        expr[n].get<expression::variable>().name_, v);
      }
      throw parse_error{"Invalid assignment target."};
    }
    return n;
  }*/

  // equality → comparison ( ( "!=" | "==" ) comparison )*
  int parse_equality(program &prog) {
    auto n = parse_comparison(prog);
    while (match({token::bang_equal, token::equal_equal})) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_comparison(expr);
      n = expr.add(std::in_place_type<expression::binary>, n, op, right);
    }
    return n;
  }

  // comparison → addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
  int parse_comparison(program &prog) {
    auto n = parse_addition(expr);
    while (match({token::greater, token::greater_equal, token::less,
                  token::less_equal})) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_addition(expr);
      n = expr.add(std::in_place_type<expression::binary>, n, op, right);
    }
    return n;
  }

  // addition → multiplication ( ( "-" | "+" ) multiplication )*
  int parse_addition(program &prog) {
    auto n = parse_multiplication(expr);
    while (match({token::minus, token::plus})) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_multiplication(expr);
      n = expr.add(std::in_place_type<expression::binary>, n, op, right);
    }
    return n;
  }

  // multiplication → unary ( ( "/" | "*" ) unary )*
  int parse_multiplication(program &prog) {
    auto n = parse_unary(expr);
    while (match({token::slash, token::star})) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_unary(expr);
      n = expr.add(std::in_place_type<expression::binary>, n, op, right);
    }
    return n;
  }

  // unary → ( "!" | "-" ) unary | call
  int parse_unary(program &prog) {
    if (match({token::bang, token::minus})) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_unary(expr);
      return expr.add(std::in_place_type<expression::unary>, op, right);
    }
    return parse_call(expr);
  }

  // call → primary ( "(" arguments? ")" )*
  /*int parse_call(expression &expr) {
    auto callee = parse_primary(expr);
    while (true) {
      if (match(token::left_paren)) {
        callee = parse_arguments(expr, callee);
      } else {
        break;
      }
    }
    return callee;
  }*/

  // arguments → expression ( "," expression )*
  /*int parse_arguments(expression &expr, int callee) {
    std::vector<int> arguments;
    if (!check(token::right_paren)) {
      do {
        if (arguments.size() >= 8) {
          throw parse_error{"Cannot have more than 8 arguments."};
        }
        arguments.emplace_back(parse_expression(expr));
      } while (match(token::comma));
    }
    auto index = expr.add(std::in_place_type<expression::call>, callee,
                          std::move(arguments));
    consume(token::right_paren, "Expect ')' after arguments.");
    return index;
  }*/

  // logic_or → logic_and ( "or" logic_and )*
  int parse_or(program &prog) {
    auto left = parse_and(expr);
    while (match(token::k_or)) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_and(expr);
      left = expr.add(std::in_place_type<expression::binary>, left, op, right);
    }
    return left;
  }

  // logic_and → equality ( "and" equality )*
  int parse_and(program &prog) {
    auto left = parse_equality(expr);
    while (match(token::k_and)) {
      auto op = expression::from_token_type(previous().get_type());
      auto right = parse_equality(expr);
      left = expr.add(std::in_place_type<expression::binary>, left, op, right);
    }
    return left;
  }

  // primary → NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")" |
  //           IDENTIFIER
  int parse_primary(program &prog) {
    if (match(token::k_false)) {
      return expr.add(std::in_place_type<expression::literal>, false);
    }
    if (match(token::k_true)) {
      return expr.add(std::in_place_type<expression::literal>, true);
    }
    if (match(token::k_nil)) {
      return expr.add(std::in_place_type<expression::literal>);
    }
    if (match({token::number, token::string})) {
      return expr.add(std::in_place_type<expression::literal>,
                      previous().get_value());
    }
    if (match(token::identifier)) {
      return expr.add(std::in_place_type<expression::variable>,
                      previous().get_lexeme());
    }
    if (match(token::left_paren)) {
      auto n = parse_expression(expr);
      consume(token::right_paren, "Expect ')' after expression.");
      return expr.add(std::in_place_type<expression::group>, n);
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

  bool check(token::type_t t) const noexcept {
    if (is_end()) return false;
    return peek().get_type() == t;
  }

  const token &advance() noexcept {
    if (!is_end()) current_++;
    return previous();
  }

  const token &consume(token::type_t t, const std::string &message) {
    if (check(t)) return advance();
    throw parse_error{message};
  }

  void synchronize() {
    advance();
    while (!is_end()) {
      if (previous().get_type() != token::semicolon) return;
      switch (peek().get_type()) {
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

  const token &peek() const noexcept { return tokens_[current_]; }
  token &previous() noexcept { return tokens_[current_ - 1]; }

  bool is_end() const noexcept { return peek().get_type() == token::eof; }

  token_vector tokens_;
  token_vector::const_iterator current_;
};

}  // namespace lox

#endif