#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include <string>
#include <type_traits>
#include <variant>

namespace lox {

using index_t = int;

struct null {};

inline bool operator==(const null &, const null &) noexcept { return true; }

using string = std::string;

using type_id_t = int;

template <typename... Ts> struct type_list {
  static constexpr type_id_t size = sizeof...(Ts);
};

template <typename T, typename types> struct type_id;

template <typename T, typename First, typename... Rest>
struct type_id<T, type_list<First, Rest...>> {
  static constexpr auto id = std::is_same_v<std::decay_t<T>, First>
                                 ? static_cast<type_id_t>(sizeof...(Rest))
                                 : type_id<T, type_list<Rest...>>::id;
};

template <typename T> struct type_id<T, type_list<>> {
  static constexpr type_id_t id = -1;
};

template <typename Variant, typename T, std::size_t index = 0>
constexpr inline std::size_t variant_index() {
  if constexpr (index == std::variant_size_v<Variant>) {
    return index;
  } else if constexpr (std::is_same_v<
                           std::variant_alternative_t<index, Variant>, T>) {
    return index;
  } else {
    return variant_index<Variant, T, index + 1>();
  }
}

template <typename... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> overloaded(Ts...)->overloaded<Ts...>;

} // namespace lox

#endif
