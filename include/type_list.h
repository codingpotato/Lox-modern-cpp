#ifndef LOX_TYPE_LIST_H
#define LOX_TYPE_LIST_H

#include <type_traits>

namespace lox {

template <typename... Ts>
struct type_list {};

template <typename T, typename Type_list>
struct index_of;

template <typename T>
struct index_of<T, type_list<>> {
  constexpr static int value = -1;
};

template <typename T, typename First, typename... Rest>
struct index_of<T, type_list<First, Rest...>> {
  constexpr static int value = std::is_same_v<T, First>
                                   ? static_cast<int>(sizeof...(Rest))
                                   : index_of<T, type_list<Rest...>>::value;
};

}  // namespace lox

#endif
