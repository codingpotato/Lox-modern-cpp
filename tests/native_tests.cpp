#include <doctest/doctest.h>

#include <thread>

#include "helper.h"

TEST_CASE("native") {
  REQUIRE(std::stod(run("print clock();")) ==
          doctest::Approx(
              static_cast<double>(
                  std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count()) /
              std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::seconds{1})
                  .count()));
}
