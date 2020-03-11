#ifndef LOX_TYPE_LIST_H
#define LOX_TYPE_LIST_H

#include <type_traits>

namespace lox {

template <typename Target, size_t N, typename... Ts>
struct index_of_impl {};

template <typename Target, size_t N, typename T, typename... Ts>
struct index_of_impl<Target, N, T, Ts...> {
  static constexpr size_t value{index_of_impl<Target, N + 1, Ts...>::value};
};

template <typename Target, size_t N, typename... Ts>
struct index_of_impl<Target, N, Target, Ts...> {
  static constexpr size_t value{N};
};

template <typename Target, typename... Ts>
struct index_of {
  static constexpr size_t value{index_of_impl<Target, 0, Ts...>::value};
};

template <typename... Ts>
struct type_list {};

template <typename Target, typename... Ts>
struct index_of<Target, type_list<Ts...>> {
  static constexpr size_t value{index_of_impl<Target, 0, Ts...>::value};
};

}  // namespace lox

#endif
