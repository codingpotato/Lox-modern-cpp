#ifndef LOX_CACHE_H
#define LOX_CACHE_H

#include <vector>

namespace lox {

template <typename T>
class cache {
 public:
  class index {
   public:
    constexpr index(int i) noexcept : idx{i} {}
    constexpr index(size_t i) noexcept : idx{static_cast<int>(i)} {}

    constexpr operator int() const noexcept { return idx; }

   private:
    int idx;
  };

  index add(T t) noexcept {
    elements.emplace_back(std::move(t));
    return elements.size() - 1;
  }

  index size() const noexcept { return elements.size(); }

  const T& get(index i) const noexcept { return elements.at(i); }

 private:
  std::vector<T> elements;
};

using double_index = cache<double>::index;
using string_index = cache<std::string>::index;

struct expression;
struct statement;
using expression_index = cache<expression>::index;
using statement_index = cache<statement>::index;

}  // namespace lox

#endif
