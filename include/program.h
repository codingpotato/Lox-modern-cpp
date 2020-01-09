#ifndef LOX_PROGRAM_H
#define LOX_PROGRAM_H

#include <vector>

#include "expression.h"
#include "statement.h"

namespace lox {

struct program {
  template <typename Type, typename... Args>
  expression_index add(Args&&... args) noexcept {
    if constexpr (std::is_same_v<Type, expression>) {
      expressions.emplace_back(std::forward<Args>(args)...);
      return expressions.size() - 1;
    } else if constexpr (std::is_same_v<Type, statement>) {
      statements.emplace_back(std::forward<Args>(args)...);
      return statements.size() - 1;
    }
  }

  expression_vector expressions;
  statement_vector statements;
};

}  // namespace lox

#endif