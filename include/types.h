#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace lox {

struct null {
  friend constexpr bool operator==(const null &, const null &) noexcept {
    return true;
  }
};

using string = std::string;

using type_id = int;

template <typename... Ts>
struct type_list {
  static constexpr type_id size = sizeof...(Ts);
};

template <typename T, typename types>
struct type_wrapper;

template <typename T, typename First, typename... Rest>
struct type_wrapper<T, type_list<First, Rest...>> {
  static constexpr auto id = std::is_same_v<std::decay_t<T>, First>
                                 ? static_cast<type_id>(sizeof...(Rest))
                                 : type_wrapper<T, type_list<Rest...>>::id;
};

template <typename T>
struct type_wrapper<T, type_list<>> {
  static constexpr type_id id = -1;
};

template <typename... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...)->overloaded<Ts...>;

}  // namespace lox

#endif
