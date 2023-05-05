#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_sine16.h"
#include "orpheus_util.hpp"

TEST_CASE("Test sin16 against Orpheus") {
  auto model = std::make_unique<Vmod_sine16>();
  for (std::int16_t i = 0; i < std::numeric_limits<int16_t>::max(); i++) {
    model->i_angle = i;
    model->eval();
    REQUIRE(static_cast<int16_t>(model->o_sine) == Math::Fixed::sin(i));
  }
}

