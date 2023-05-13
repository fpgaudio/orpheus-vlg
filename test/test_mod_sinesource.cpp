#include "orpheus.hpp"
#include "test/test_utils.hpp"
#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>
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

  Plotting::Signal<int32_t, int32_t> i_periods { "i\\\\_periods" };
  Plotting::Signal<int32_t, bool> i_nrsts { "i\\\\_nrsts" };
  Plotting::DigitalSignal<int32_t, int32_t> i_clks { "i\\\\_clks" };
  Plotting::Signal<int32_t, int16_t> o_sines { "o\\\\_sines" };
  Plotting::DigitalSignal<int32_t, bool> o_readys { "o\\\\_readys" };

  // For a period of 100
  std::size_t sampleCounter = 0;
  for (std::int32_t time = 0; time < 100; time++) {
    for (auto period : { 100 }) {
      for (auto clock : { 0, 1, 0, 1 }) {
        for (auto nrst : { 1 }) {
          model->i_time = time;
          model->i_period = period;
          model->i_clk  = clock;
          model->i_nrst = nrst;
          model->eval();

          i_periods.push_back({ sampleCounter, model->i_period });
          i_clks.push_back({ sampleCounter, model->i_clk });
          i_nrsts.push_back({ sampleCounter, model->i_nrst });
          o_sines.push_back({ sampleCounter, model->o_sine });
          o_readys.push_back({ sampleCounter, model->o_ready });
          sampleCounter++;
        }
      }
    }
  }

  Plotting::plot({ &i_periods, &i_nrsts, &i_clks, &o_sines, &o_readys });
}
