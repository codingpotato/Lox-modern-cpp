#include <array>
#include <algorithm>

#include <doctest.h>

#include "variant.h"
#include "types.h"
#include "performance.h"

TEST_CASE("variant operation") {
  lox::variant<int, double> value{1};
  CHECK(value.is_type<int>());
  CHECK(!value.is_type<double>());
  CHECK_EQ(value.as<int>(), 1);
  CHECK_THROWS(value.as<double>());
  value.visit(lox::overloaded{
      [](auto i) { CHECK_EQ(i, 1); },
  });
}

TEST_CASE("varianr visit performance") {
  struct One {
    auto get() const { return 1; }
  };
  struct Two {
    auto get() const { return 2; }
  };
  struct Three {
    auto get() const { return 3; }
  };
  struct Four {
    auto get() const { return 4; }
  };

  using value_t = lox::variant<One, Two, Three, Four>;

  constexpr int value_count = 10000000;
  std::vector<value_t> values(value_count);
  std::generate(values.begin(), values.end(), []() -> value_t {
    switch (std::rand() % 4) {
      case 0:
        return One{};
      case 1:
        return Two{};
      case 2:
        return Three{};
      case 3:
        return Four{};
      default:
        return One{};
    }
  });

  SUBCASE("visit") {
    volatile int result = 0;
    measure("Variant visit", [&]() {
      for (int i = 0; i < value_count; ++i) {
        values[i % values.size()].visit(lox::overloaded{
            [&](const One& r) { result += r.get(); },
            [&](const Two& r) { result += r.get(); },
            [&](const Three& r) { result += r.get(); },
            [&](const Four& r) { result += r.get(); },
        });
      }
    });
  }
}
