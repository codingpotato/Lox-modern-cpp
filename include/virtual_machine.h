#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <stack>
#include <vector>

#include "resolver.h"
#include "statement.h"

namespace lox {

class virtual_machine {
 public:
  bool is_current_block_done() const noexcept {
    return current.first == current.last;
  }

  statement_id current_statement() const noexcept { return current.first; }
  void advance() noexcept { ++current.first; }

  void excute_block(statement_id first, statement_id last) noexcept {
    block_stack.push(current);
    current = {first, last};
  }

  void pop_block() noexcept {
    if (!block_stack.empty()) {
      current = block_stack.top();
      block_stack.pop();
    }
  }

  void begin_scope() noexcept { value_stack.push_back({}); }
  void end_scope() noexcept { value_stack.pop_back(); }

  void define_value(value v) noexcept {
    Expect(!value_stack.empty());
    value_stack.back().emplace_back(std::move(v));
  }

  value get(resolve_info info) const noexcept {
    return value_stack[value_stack.size() - 1 - info.distance][info.index];
  }

 private:
  struct block_info {
    statement_id first;
    statement_id last;
  };

  using value_frame = std::vector<value>;

  block_info current;
  std::stack<block_info> block_stack;
  std::vector<value_frame> value_stack;
};  // namespace lox

}  // namespace lox

#endif
