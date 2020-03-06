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
  auto visit(Visitor&& visitor) const {
    return std::visit(std::forward<Visitor>(visitor), storage);
  }

 private:
  std::variant<Ts...> storage;
};

}  // namespace lox

#endif
