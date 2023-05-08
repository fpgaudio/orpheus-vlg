#include "test/test_utils.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_synth.h"
#include "orpheus_util.hpp"
#include <variant>

TEST_CASE("Test synthesizer") {
  auto model = std::make_unique<Vmod_synth>();

  LogFile timeLog { "mod_synth_i_time.txt" };
  LogFile atten1Log { "mod_synth_i_atten1.txt" };
  LogFile atten2Log { "mod_synth_i_atten2.txt" };
  LogFile atten3Log { "mod_synth_i_atten3.txt" };
  LogFile atten4Log { "mod_synth_i_atten4.txt" };
  LogFile attenOutLog { "mod_synth_i_attenOut.txt" };
  LogFile periodLog { "mod_synth_i_period.txt" };
  LogFile oSoundLog { "mod_synth_o_sound.txt" };
  CsvLogFile oCsvLog { "mod_synth_test.csv",
    { "i_time", "i_atten1", "i_atten2", "i_atten3", "i_atten4", "i_atten_out", "i_period",
      "o_sound" }
  };

  const auto period = 100;
  const auto atten1 = std::numeric_limits<int16_t>::max() / 4;
  const auto atten2 = std::numeric_limits<int16_t>::max() / 4;
  const auto atten3 = std::numeric_limits<int16_t>::max() / 4;
  const auto atten4 = std::numeric_limits<int16_t>::max() / 4;
  const auto attenOut = std::numeric_limits<int16_t>::max();
  for (std::int16_t time = 0; time < std::numeric_limits<int16_t>::max(); time++) {
    model->i_time = time;
    model->i_atten1 = atten1;
    model->i_atten2 = atten2;
    model->i_atten3 = atten3;
    model->i_atten4 = atten4;
    model->i_atten_out = attenOut;
    model->i_period = period;
    model->eval();

    timeLog.dump(model->i_time);
    atten1Log.dump(model->i_atten1);
    atten2Log.dump(model->i_atten2);
    atten3Log.dump(model->i_atten3);
    atten4Log.dump(model->i_atten4);
    attenOutLog.dump(model->i_atten_out);
    periodLog.dump(model->i_period);
    oSoundLog.dump(model->o_sound);

    oCsvLog.dump(std::vector({
      static_cast<int32_t>(model->i_time),
      static_cast<int32_t>(model->i_atten1),
      static_cast<int32_t>(model->i_atten2),
      static_cast<int32_t>(model->i_atten3),
      static_cast<int32_t>(model->i_atten4),
      static_cast<int32_t>(model->i_atten_out),
      static_cast<int32_t>(model->i_period),
      static_cast<int32_t>(model->o_sound)
    }));
  }

  model->final();
}
