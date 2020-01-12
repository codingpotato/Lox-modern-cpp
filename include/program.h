#ifndef LOX_PROGRAM_H
#define LOX_PROGRAM_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "exception.h"
#include "expression.h"
#include "statement.h"

namespace lox {

class string_cache {
 public:
  string_id add(std::string string) {
    auto str = std::move(string);
    const auto [iterator, inserted] = strings.emplace(str, current);
    if (inserted) {
      const auto &new_str = iterator->first;
      ids.emplace(current, std::string_view{new_str.data(), new_str.size()});
      ++current;
      if (current == 0) {
        throw internal_error{"String cache overflow."};
      }
      return current - 1;
    }
    return strings[str];
  }

  string_id size() const noexcept { return current; }

  std::string_view get(string_id id) const noexcept { return ids.at(id); }

 private:
  std::unordered_map<std::string, string_id> strings;
  std::unordered_map<string_id, std::string_view> ids;
  string_id current = 0;
};

struct program {
  template <typename Type, typename... Args>
  index_t add(Args &&... args) {
    if constexpr (std::is_same_v<Type, expression>) {
      expressions.emplace_back(std::forward<Args>(args)...);
      return expressions.size() - 1;
    } else if constexpr (std::is_same_v<Type, statement>) {
      statements.emplace_back(std::forward<Args>(args)...);
      return statements.size() - 1;
    } else {
      throw internal_error("Type shall be expression or statement.");
    }
  }

  template <typename Type>
  index_t size() const noexcept {
    if constexpr (std::is_same_v<Type, expression>) {
      return expressions.size();
    } else if constexpr (std::is_same_v<Type, statement>) {
      return statements.size();
    } else {
      throw internal_error("Type shall be expression or statement.");
    }
  }

  template <typename Type>
  const auto &get(index_t index) const {
    if constexpr (std::is_same_v<Type, expression>) {
      return expressions.at(index);
    } else if constexpr (std::is_same_v<Type, statement>) {
      return statements.at(index);
    } else {
      throw internal_error("Type shall be expression or statement.");
    }
  }

  string_cache strings;
  expression_vector expressions;
  statement_vector statements;
  index_t start_block;
};

}  // namespace lox

#endif