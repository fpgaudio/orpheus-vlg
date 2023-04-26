#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vorpheus_sine16.h"

TEST_CASE("Test sin16 against Orpheus") {
  auto model = std::make_unique<Vorpheus_sine16>();
  for (std::int16_t i = 0; i < std::numeric_limits<int16_t>::max(); i++) {
    model->angle_input = i;
    model->eval();

    const auto actualSine = model->o;

    const auto expectedSine = sin(
      2 * std::numbers::pi *
      static_cast<double>(i) / static_cast<double>(std::numeric_limits<int16_t>::max())
    );

    REQUIRE(static_cast<double>(static_cast<int16_t>(actualSine)) / 4096.0
      == Approx(expectedSine).margin(2 / 4096.0));
  }
}

