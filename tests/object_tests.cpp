#include <doctest.h>

#include "object.h"
#include "performance.h"

TEST_CASE("object performance") {
  volatile const std::string* result = nullptr;
  lox::measure("object performance", 100000, [&]() {
    lox::object string{"test string"};
    result = &string.as<std::string>();
  });
}
