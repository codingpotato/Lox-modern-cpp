#ifndef LOX_STATEMENT_H
#define LOX_STATEMENT_H

#include <string>
#include <variant>
#include <vector>

#include "expression.h"
#include "types.h"

namespace lox {

class statement;
using statements = std::vector<statement>;

struct statement {
  struct block {
    template <typename S>
    constexpr explicit block(S&& ss) noexcept
        : statements_{std::forward<S>(ss)} {}

    statements statements_;
  };

  struct expr {
    template <typename E>
    constexpr explicit expr(E&& expr) noexcept : expr_{std::forward<E>(expr)} {}

    expression expr_;
  };

  struct function {
    template <typename N, typename P, typename B>
    constexpr function(N&& name, P&& parameters, B&& body) noexcept
        : name_{std::forward<N>(name)},
          parameters_{std::forward<P>(parameters)},
          body_{std::forward<B>(body)} {}

    std::string name_;
    std::vector<std::string> parameters_;
    statements body_;
  };

  struct if_else {
    template <typename E, typename S1, typename S2>
    constexpr if_else(E&& condition, S1&& ts, S2&& es) noexcept
        : condition_{std::forward<E>(condition)},
          then_{std::forward<S1>(ts)},
          else_{std::forward<S2>(es)} {}

    expression condition_;
    statements then_;
    statements else_;
  };

  struct return_s {
    template <typename E>
    constexpr explicit return_s(E&& value) noexcept
        : value_{std::forward<E>(value)} {}

    expression value_;
  };

  struct var {
    template <typename S, typename E>
    constexpr var(S&& name, E&& initializer) noexcept
        : name_{std::forward<S>(name)},
          initializer_{std::forward<E>(initializer)} {}

    std::string name_;
    expression initializer_;
  };

  struct while_loop {
    template <typename E, typename S>
    constexpr while_loop(E&& condition, S&& ss) noexcept
        : condition_{std::forward<E>(condition)}, ss_{std::forward<S>(ss)} {}

    expression condition_;
    statements ss_;
  };

  template <typename T, typename... Args>
  constexpr explicit statement(std::in_place_type_t<T> t,
                               Args&&... args) noexcept
      : statement_{t, std::forward<Args>(args)...} {}

  template <typename T>
  constexpr bool is_type() const noexcept {
    return std::holds_alternative<T>(statement_);
  }

  template <typename T>
  auto& get() noexcept {
    return std::get<T>(statement_);
  }

  template <typename... Ts>
  auto visit(overloaded<Ts...>&& visitor) const {
    return std::visit(visitor, statement_);
  }

  using statement_t =
      std::variant<block, expr, function, if_else, return_s, var, while_loop>;
  statement_t statement_;
};

}  // namespace lox

#endif