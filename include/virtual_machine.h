#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <stack>
#include <vector>

#include "statement.h"

namespace lox {

class virtual_machine {
 public:
  bool done() const noexcept { return ip.current == statement_id{}; }
  bool is_in_current_block() const noexcept { return ip.current < ip.last; }

  statement_id current_statement() const noexcept { return ip.current; }
  void next_statement() noexcept { ++ip.current; }

  void excute_block(statement_id first, statement_id last,
                    bool advance_current) noexcept {
    if (advance_current && ip.current != statement_id{}) {
      ++ip.current;
      ip_stack.push(ip);
    }
    ip = {first, last};
  }

  void pop_block() noexcept {
    if (!ip_stack.empty()) {
      ip = ip_stack.top();
      ip_stack.pop();
    } else {
      ip = instruction_pointer{};
    }
  }

 private:
  struct instruction_pointer {
    statement_id current;
    statement_id last;
  };

  using value_frame = std::vector<value>;

  instruction_pointer ip;
  std::stack<instruction_pointer> ip_stack;
  std::vector<value_frame> value_stack;
};

}  // namespace lox

#endif
