#ifndef LOX_VARIANT_STORAGE_H
#define LOX_VARIANT_STORAGE_H

namespace lox {

template <typename Variant_element>
struct variant_storage {
  template <typename... Args>
  constexpr variant_storage(Args &&... args) noexcept
      : element{std::forward<Args>(args)...} {}

  template <typename T>
  constexpr bool is_type() const noexcept {
    return std::holds_alternative<T>(element);
  }

  template <typename T>
  const auto &get() const noexcept {
    return std::get<T>(element);
  }

  template <typename... Ts>
  auto visit(overloaded<Ts...> &&visitor) const {
    return std::visit(visitor, element);
  }

  Variant_element element;
};

}  // namespace lox

#endif