#include "orpheus.hpp"
#include "test/test_utils.hpp"
#include <cstdint>
#include <fstream>
#include <limits>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_attenuator.h"
#include "orpheus_util.hpp"

TEST_CASE("Test attenutator against Orpheus") {
  auto model = std::make_unique<Vmod_attenuator>();

  LogFile inputStream { "mod_attenuator_i_raw.txt" };
  LogFile attenuationStream { "mod_attenuator_i_attenfactor.txt" };
  LogFile outputStream { "mod_attenuator_o_attenuated.txt" };

  for (std::int16_t i = 0; i < 30; i++) {
    model->i_attenfactor = i;
    for (std::int16_t j = 0; j < std::numeric_limits<int16_t>::max(); j++) {
      model->i_raw = j;
      model->eval();

      REQUIRE(
        static_cast<int16_t>(model->o_attenuated) == (j * i) / std::numeric_limits<int16_t>::max()
      );

      attenuationStream.dump(model->i_attenfactor);
      inputStream.dump(model->i_raw);
      outputStream.dump(model->o_attenuated);
    }
  }
}
