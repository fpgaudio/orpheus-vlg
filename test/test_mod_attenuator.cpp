#include "orpheus.hpp"
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

  std::ofstream inputStream;
  inputStream.open("mod_attenuator_i_raw.txt");
  std::ofstream attenuationStream;
  attenuationStream.open("mod_attenuator_i_attenfactor.txt");
  std::ofstream outputStream;
  outputStream.open("mod_attenuator_o_attenuated.txt");

  for (std::int16_t i = 0; i < 30; i++) {
    model->i_attenfactor = i;
    for (std::int16_t j = 0; j < std::numeric_limits<int16_t>::max(); j++) {
      model->i_raw = j;
      model->eval();

      REQUIRE(
        static_cast<int16_t>(model->o_attenuated) == (j * i) / std::numeric_limits<int16_t>::max()
      );

      inputStream << std::setw(4) << std::setfill('0') << std::hex << model->i_attenfactor << std::endl;
      inputStream << std::setw(4) << std::setfill('0') << std::hex << model->i_raw << std::endl;
      outputStream << std::setw(4) << std::setfill('0') << std::hex << model->o_attenuated << std::endl;
    }
  }

  inputStream.close();
  attenuationStream.close();
  outputStream.close();
}
