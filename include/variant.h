#ifndef LOX_VARIANT_H
#define LOX_VARIANT_H

#include <variant>
#include <iostream>

namespace lox {

template <typename... Ts>
struct variant {
  static_assert(sizeof...(Ts) > 0);

  template <typename... Args>
  constexpr variant(Args&&... args) noexcept
      : storage{std::forward<Args>(args)...} {}

  template <typename T>
  constexpr bool is_type() const noexcept {
    return std::holds_alternative<T>(storage);
  }

  template <typename T>
  constexpr const T& as() const {
    return std::get<T>(storage);
  }

  template <typename Visitor>
  void visit(Visitor&& visitor) const {
    visit<Visitor, sizeof...(Ts) - 1>(std::forward<Visitor>(visitor));
  }

  /*
   * std::visit implement is slower than std::get_if
   */
  template <typename Visitor>
  void std_visit(Visitor&& visitor) const {
    std::visit(std::forward<Visitor>(visitor), storage);
  }

 private:
  template <typename Visitor, std::size_t N>
  void visit(Visitor&& visitor) const {
    if (auto value = std::get_if<N>(&storage)) {
      std::forward<Visitor>(visitor)(*value);
    } else if constexpr (N > 0) {
      visit<Visitor, N - 1>(std::forward<Visitor>(visitor));
    }
  }

  std::variant<Ts...> storage;
};

}  // namespace lox

#endif
