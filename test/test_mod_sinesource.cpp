#include "orpheus.hpp"
#include <cstdint>
#include <limits>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_sinesource.h"
#include "orpheus_util.hpp"

TEST_CASE("Test sinesource against Orpheus") {
  Orpheus::Engine engine { 1 };
  Orpheus::Graph::SineSource sine { &engine };
  sine.setPeriodTicks(100);

  auto model = std::make_unique<Vmod_sinesource>();
  for (std::int32_t tick = 0; tick < std::numeric_limits<int32_t>::max(); tick++) {
    model->i_period = 100;
    model->i_time = tick;
    model->eval();
    std::cout << model->o_sine << " == " << sine() << std::endl;
    REQUIRE(static_cast<int32_t>(model->o_sine) == sine());
    engine.tick();
  }
}
