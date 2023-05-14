#include "test/test_utils.hpp"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_sinesource.h"
#include <numbers>

class ClockProvider
{
public:
  inline void toggleUntil(std::function<bool (bool)> predicate, size_t maxCycles = 10000) {
    // A toggle is only half a cycle --v
    for (size_t i = 0; i < maxCycles * 2; i++, toggle()) {
      if (predicate(m_state)) {
        return;
      }
    }
    std::cerr << "Warning: ClockProvider reached the maximum number of cycles " << maxCycles
              << "without satisfying the given condition." << std::endl;
  }
  constexpr void toggle() {
    m_state = !m_state;
  }
  constexpr void pulse() {
    toggle();
    toggle();
  }
  constexpr bool getState() { return m_state; }

private:
  bool m_state = 0;
};

template<typename T, size_t mantissaWidth, typename FloatLike>
T float2fixed(FloatLike val) {
  static_assert(
    mantissaWidth <= std::numeric_limits<FloatLike>::digits,
    "The given type does not support the specified mantissa width. Seems like an error."
  );
  return static_cast<T>(std::round(val * (1 << mantissaWidth)));
}

template<size_t mantissaWidth, typename FloatLike, typename T>
FloatLike fixed2float(FloatLike val) {
  static_assert(
    mantissaWidth <= std::numeric_limits<FloatLike>::digits,
    "The given type does not support the specified mantissa width. Seems like an error."
  );

  return static_cast<FloatLike>(val) / std::pow(2, mantissaWidth);
}

void seedEvalTest(
  Vmod_sinesource* model,
  auto time,
  uint32_t frequency,
  auto clock,
  auto nrst,
  auto trigger,
  auto& i_times,
  auto& i_frequencys,
  auto& i_clks,
  auto& i_nrsts,
  auto& i_triggers,
  auto& o_sines,
  auto& o_sines_expected,
  auto& o_readys,
  auto& sampleCounter
) {
  const double frequencyFloat = fixed2float<15, double, uint32_t>(frequency);

  model->i_time = time;
  model->i_frequency = frequency;
  model->i_trigger = trigger;
  model->i_clk = clock;
  model->i_nrst = nrst;

  // Evaluate the model, push the data back.
  model->eval();

  // Update the graphing signals
  i_times.push_back({ sampleCounter, model->i_time });
  i_frequencys.push_back({ sampleCounter, model->i_frequency });
  i_clks.push_back({ sampleCounter, model->i_clk });
  i_nrsts.push_back({ sampleCounter, model->i_nrst });
  i_triggers.push_back({ sampleCounter, model->i_trigger });
  o_sines.push_back({ sampleCounter, model->o_sine });
  o_sines_expected.push_back({ sampleCounter,
      std::sin(2 * std::numbers::pi * time * frequencyFloat)
  });
  o_readys.push_back({ sampleCounter, model->o_ready });
  sampleCounter++;
}

TEST_CASE("Test sinesource against Orpheus") {
  auto context = std::make_unique<VerilatedContext>();
  Verilated::traceEverOn(true);
  auto tf = std::make_unique<VerilatedVcdC>();

  LogFile frequencyLog { "mod_sinesource_i_frequency.txt" };
  LogFile timeLog { "mod_sinesource_i_time.txt" };
  LogFile sineLog { "mod_sinesource_o_sine.txt" };

  auto model = std::make_shared<Vmod_sinesource>();
  model->trace(tf.get(), 99);

  tf->open("test.vcd");

  Plotting::Signal<int32_t, int32_t> i_times { "i\\\\_times" };
  Plotting::Signal<int32_t, int32_t> i_frequencys { "i\\\\_frequencys" };
  Plotting::Signal<int32_t, bool> i_nrsts { "i\\\\_nrsts" };
  Plotting::DigitalSignal<int32_t, int32_t> i_clks { "i\\\\_clks" };
  Plotting::DigitalSignal<int32_t, bool> i_triggers { "i\\\\_triggers" };
  Plotting::Signal<int32_t, int16_t> o_sines { "o\\\\_sines" };
  Plotting::Signal<int32_t, double> o_sines_expected { "o\\\\_sines\\\\_expected" };
  Plotting::DigitalSignal<int32_t, bool> o_readys { "o\\\\_readys" };

  // For a frequency of 100
  ClockProvider clk;
  std::size_t sampleCounter = 0;
  // 
  for (std::int32_t time = 0; time < 1000; time += 1) {
    for (auto frequencyF : { 9.166e-3 }) {
      for (auto nrst : { 1 }) {
        const auto frequency = float2fixed<uint32_t, 15>(frequencyF);
        // Firstly, seed the model with no trigger.
        seedEvalTest(
          model.get(), time, frequency, clk.getState(), nrst, 0,
          i_times, i_frequencys, i_clks, i_nrsts, i_triggers, o_sines, o_sines_expected, o_readys,
          sampleCounter
        );
        clk.toggle();
        context->timeInc(1);
        tf->dump(context->time());
        seedEvalTest(
          model.get(), time, frequency, clk.getState(), nrst, 0,
          i_times, i_frequencys, i_clks, i_nrsts, i_triggers, o_sines, o_sines_expected, o_readys,
          sampleCounter
        );
        clk.toggle();
        context->timeInc(1);
        tf->dump(context->time());
        // Then, fire trigger high
        seedEvalTest(
          model.get(), time, frequency, clk.getState(), nrst, 1,
          i_times, i_frequencys, i_clks, i_nrsts, i_triggers, o_sines, o_sines_expected, o_readys,
          sampleCounter
        );
        clk.toggle();
        context->timeInc(1);
        tf->dump(context->time());
        seedEvalTest(
          model.get(), time, frequency, clk.getState(), nrst, 1,
          i_times, i_frequencys, i_clks, i_nrsts, i_triggers, o_sines, o_sines_expected, o_readys,
          sampleCounter
        );
        clk.toggle();
        context->timeInc(1);
        tf->dump(context->time());

        // Then, wait until the model fires o_ready
        size_t loopI = 0;
        while (model->o_ready != 1 && (loopI++) < 1000) {
          // Let us seed the model with the current state.
          seedEvalTest(
            model.get(), time, frequency, clk.getState(), nrst, 0,
            i_times, i_frequencys, i_clks, i_nrsts, i_triggers, o_sines, o_sines_expected, o_readys,
            sampleCounter
          );

          clk.toggle();
          context->timeInc(1);
          tf->dump(context->time());
        };
      }
    }
  }

  Plotting::plot({
    &i_times,
    &i_frequencys,
    &i_nrsts,
    &i_clks,
    &i_triggers,
    &o_sines,
    &o_sines_expected,
    &o_readys
  });

  tf->close();
}
