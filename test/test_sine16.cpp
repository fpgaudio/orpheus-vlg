#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vorpheus_sine16.h"
#include "orpheus_util.hpp"

TEST_CASE("Test sin16 against Orpheus") {
  auto model = std::make_unique<Vorpheus_sine16>();
  for (std::int16_t i = 0; i < std::numeric_limits<int16_t>::max(); i++) {
    model->angle_input = i;
    model->eval();
    REQUIRE(static_cast<int16_t>(model->o) == Math::Fixed::sin(i));
  }
}

