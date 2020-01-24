#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <stack>
#include <vector>

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
