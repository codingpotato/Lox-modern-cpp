#ifndef LOX_CACHE_H
#define LOX_CACHE_H

#include <vector>

#include "types.h"

namespace lox {

template <typename T>
class cache {
 public:
  class index {
   public:
    constexpr index() noexcept {}
    constexpr index(int i) noexcept : idx{i} {}
    constexpr index(size_t i) noexcept : idx{static_cast<int>(i)} {}

    constexpr operator int() const noexcept { return idx; }

   private:
    int idx = -1;
  };

  using vector = std::vector<T>;
  using const_iterator = typename vector::const_iterator;

  template <typename... Args>
  index add(Args... args) noexcept {
    elements.emplace_back(std::move(args)...);
    return elements.size() - 1;
  }

  void add(const_iterator first, const_iterator last) noexcept {
    std::copy(first, last, std::back_inserter(elements));
  }

  index size() const noexcept { return elements.size(); }

  const T& get(index i) const noexcept { return elements.at(i); }

 private:
  std::vector<T> elements;
};

using double_index = cache<double>::index;
using string_index = cache<string>::index;

struct expression;
struct statement;
using expression_index = cache<expression>::index;
using statement_index = cache<statement>::index;

}  // namespace lox

#endif
