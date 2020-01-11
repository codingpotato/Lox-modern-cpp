#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <iostream>
#include <vector>

#include "exception.h"
#include "expression.h"
#include "program.h"
#include "statement.h"
#include "token.h"

namespace lox {

class parser {
 public:
  explicit parser(token_vector ts) noexcept : tokens{std::move(ts)} {
    current = tokens.cbegin();
  }

  /*program parse() {
    program prog;
    while (!is_end()) {
      parse_declaration(prog);
    }
    return prog;
  }*/

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
  }*/

  // expression → assignment
  int parse_expression(program &) { return 0; /*parse_assignment(prog); */ }

  // assignment → IDENTIFIER "=" assignment | logic_or
  /*int parse_assignment(program &prog) {
    auto n = parse_or(prog);
    if (match(token::equal)) {
      auto v = parse_assignment(prog);
      if (expr[n].is_type<expression::variable>()) {
        return expr.add(std::in_place_type<expression::assignment>,
                        expr[n].get<expression::variable>().name_, v);
      }
      throw parse_error{"Invalid assignment target."};
    }
    return n;
  }*/

  // equality → comparison ( ( "!=" | "==" ) comparison )*
  index_t parse_equality(program &prog) {
    auto n = parse_comparison(prog);
    while (match({token::bang_equal, token::equal_equal})) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_comparison(prog);
      n = prog.add<expression>(std::in_place_type<expression::binary>, n, op,
                               right);
    }
    return n;
  }

  // comparison → addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
  index_t parse_comparison(program &prog) {
    auto n = parse_addition(prog);
    while (match({token::greater, token::greater_equal, token::less,
                  token::less_equal})) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_addition(prog);
      n = prog.add<expression>(std::in_place_type<expression::binary>, n, op,
                               right);
    }
    return n;
  }

  // addition → multiplication ( ( "-" | "+" ) multiplication )*
  index_t parse_addition(program &prog) {
    std::cout << "start addition\n";
    auto n = parse_multiplication(prog);
    std::cout << "after multiplication\n";
    while (match({token::minus, token::plus})) {
      std::cout << "after while\n";
      auto op = expression::from_token_type(previous().type);
      auto right = parse_multiplication(prog);
      n = prog.add<expression>(std::in_place_type<expression::binary>, n, op,
                               right);
    }
    std::cout << "before return\n";
    return n;
  }

  // multiplication → unary ( ( "/" | "*" ) unary )*
  index_t parse_multiplication(program &prog) {
    std::cout << "start multiplication\n";
    auto n = parse_unary(prog);
    std::cout << "after unary\n";
    while (match({token::slash, token::star})) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_unary(prog);
      n = prog.add<expression>(std::in_place_type<expression::binary>, n, op,
                               right);
    }
    return n;
  }

  // unary → ( "!" | "-" ) unary | call
  index_t parse_unary(program &prog) {
    std::cout << "start unary\n";
    if (match({token::bang, token::minus})) {
      std::cout << "in if\n";
      auto op = expression::from_token_type(previous().type);
      auto right = parse_unary(prog);
      return prog.add<expression>(std::in_place_type<expression::unary>, op,
                                  right);
    }
    return parse_call(prog);
  }

  // call → primary ( "(" arguments? ")" )*
  index_t parse_call(program &prog) {
    std::cout << "start call\n";
    auto callee = parse_primary(prog);
    std::cout << "after primary\n";
    while (true) {
      std::cout << "after while\n";
      if (match(token::left_paren)) {
        callee = parse_arguments(prog, callee);
      } else {
        break;
      }
    }
    return callee;
  }

  // arguments → expression ( "," expression )*
  index_t parse_arguments(program &prog, index_t callee) {
    index_vector arguments;
    if (!check(token::right_paren)) {
      do {
        if (arguments.size() >= 8) {
          throw parse_error{"Cannot have more than 8 arguments."};
        }
        arguments.emplace_back(parse_expression(prog));
      } while (match(token::comma));
    }
    auto index = prog.add<expression>(std::in_place_type<expression::call>,
                                      callee, std::move(arguments));
    consume(token::right_paren, "Expect ')' after arguments.");
    return index;
  }

  // logic_or → logic_and ( "or" logic_and )*
  int parse_or(program &prog) {
    auto left = parse_and(prog);
    while (match(token::k_or)) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_and(prog);
      left = prog.add<expression>(std::in_place_type<expression::binary>, left,
                                  op, right);
    }
    return left;
  }

  // logic_and → equality ( "and" equality )*
  int parse_and(program &prog) {
    auto left = parse_equality(prog);
    while (match(token::k_and)) {
      auto op = expression::from_token_type(previous().type);
      auto right = parse_equality(prog);
      left = prog.add<expression>(std::in_place_type<expression::binary>, left,
                                  op, right);
    }
    return left;
  }

  // primary → NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")" |
  //           IDENTIFIER
  index_t parse_primary(program &prog) {
    std::cout << "in primary\n";
    if (match(token::k_false)) {
      return prog.add<expression>(false);
    }
    if (match(token::k_true)) {
      return prog.add<expression>(true);
    }
    if (match(token::k_nil)) {
      return prog.add<expression>(expression::literal{});
    }
    if (match(token::number)) {
      std::cout << "in number\n";
      auto &literal = previous().value;
      if (std::holds_alternative<int>(literal.value)) {
        return prog.add<expression>(std::get<int>(literal.value));
      }
      if (std::holds_alternative<double>(literal.value)) {
        return prog.add<expression>(std::get<double>(literal.value));
      }
    }
    if (match(token::string)) {
      auto id = prog.add_string(std::get<std::string>(previous().value.value));
      return prog.add<expression>(id);
    }
    if (match(token::identifier)) {
      return prog.add<expression>(prog.add_string(previous().lexeme));
    }
    if (match(token::left_paren)) {
      auto n = parse_expression(prog);
      consume(token::right_paren, "Expect ')' after expression.");
      return prog.add<expression>(n);
    }
    throw parse_error{"Expect expression."};
  }

  bool match(const std::initializer_list<token::type_t> &types) noexcept {
    std::cout << "in match\n";
    for (auto t : types) {
      std::cout << "in for\n";
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