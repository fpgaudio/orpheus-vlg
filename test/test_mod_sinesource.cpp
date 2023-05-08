#include "orpheus.hpp"
#include "test/test_utils.hpp"
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

  LogFile periodLog { "mod_sinesource_i_period.txt" };
  LogFile timeLog { "mod_sinesource_i_time.txt" };
  LogFile sineLog { "mod_sinesource_o_sine.txt" };

  auto model = std::make_unique<Vmod_sinesource>();
  for (auto period : { 100 }) {
    for (std::int32_t tick = 0; tick < std::numeric_limits<int16_t>::max(); tick++) {
      model->i_period = period;
      model->i_time = tick;
      model->eval();
      REQUIRE(static_cast<int32_t>(model->o_sine) == sine());
      engine.tick();

      periodLog.dump(model->i_period);
      timeLog.dump(model->i_time);
      sineLog.dump(model->o_sine);
    }
  }
}
