#ifndef LOX_PROGRAM_H
#define LOX_PROGRAM_H

#include <variant>
#include <vector>

#include "expression.h"
#include "statement.h"

namespace lox {

struct int_t {};
struct double_t {};
struct string_t {};

struct literal_index {
  std::variant<int_t, double_t, string_t> type;
  index_t index;
};

struct literal_vector {
  template <typename T>
  literal_index push_back(T &&value) noexcept {
    if constexpr (std::is_same_v<T, int>) {
      int_values.push_back(value);
      return {int_t{}, static_cast<index_t>(int_values.size()) - 1};
    } else if constexpr (std::is_same_v<T, double>) {
      double_values.push_back(value);
      return {double_t{}, static_cast<index_t>(double_values.size()) - 1};
    } else if constexpr (std::is_same_v<T, std::string>) {
      string_values.push_back(value);
      return {string_t{}, static_cast<index_t>(string_values.size()) - 1};
    }
  }

  template <typename T>
  const auto &get(index_t index) const noexcept {
    if constexpr (std::is_same_v<T, int>) {
      return int_values[index];
    } else if constexpr (std::is_same_v<T, double>) {
      return double_values[index];
    } else if constexpr (std::is_same_v<T, std::string>) {
      return string_values[index];
    }
  }

  std::vector<int> int_values;
  std::vector<double> double_values;
  std::vector<std::string> string_values;
};

struct program {
  template <typename T>
  literal_index add_literal(T &&value) noexcept {
    return literals.push_back(value);
  }

  template <typename T>
  const T &get(index_t index) noexcept {
    return literals.get<T>(index);
  }

  template <typename Type, typename... Args>
  index_t add(Args &&... args) noexcept {
    if constexpr (std::is_same_v<Type, expression>) {
      expressions.emplace_back(std::forward<Args>(args)...);
      return expressions.size() - 1;
    } else if constexpr (std::is_same_v<Type, statement>) {
      statements.emplace_back(std::forward<Args>(args)...);
      return statements.size() - 1;
    }
  }

  literal_vector literals;
  expression_vector expressions;
  statement_vector statements;
};

}  // namespace lox

#endif