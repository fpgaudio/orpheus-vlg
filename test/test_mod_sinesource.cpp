#include "test/model_tester.hpp"
#include "test/test_utils.hpp"
#include <cmath>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_sinesource.h"
#include <numbers>

class TestFixture : public Svt::AbstractTest<Vmod_sinesource, true>
{
public:
  TestFixture(VerilatedContext* ctx, const char* vcdFileName)
    : Svt::AbstractTest<Vmod_sinesource, true>(ctx, vcdFileName) {}

  virtual void populateInputs(Vmod_sinesource* model) override {
    model->i_time = getParameter<Svt::U64RangeParam>("time")->get();
    model->i_frequency = getParameter<Svt::U32RangeParam>("frequency")->get();
    model->i_trigger = m_trigger;
    model->i_clk = getClock().getState();
    model->i_nrst = getParameter<Svt::BoolRangeParam>("nrst")->get();
  }

  virtual void seedParameters() override {
    getParameters().emplace(std::make_pair("time", std::make_unique<Svt::U64RangeParam>(0, 1000)));
    getParameters().emplace(std::make_pair(
      "frequency",
      std::make_unique<Svt::U32Param>(440))
    );
    getParameters().emplace(std::make_pair("nrst", std::make_unique<Svt::TrueParam>()));
  }

  virtual void onStep(Vmod_sinesource* model) override {
    m_i_time.push_back({ getContext()->time(), model->i_time });
    m_i_frequency.push_back({ getContext()->time(), model->i_frequency });
    m_i_trigger.push_back({ getContext()->time(), model->i_trigger });
    m_i_clk.push_back({ getContext()->time(), model->i_clk });
    m_i_nrst.push_back({ getContext()->time(), model->i_nrst });
    m_o_sine.push_back({ getContext()->time(), Svt::fixed2float<14, double>(model->o_sine) });
    m_o_sine_expected.push_back({
      getContext()->time(),
      std::sin(model->i_time / 48000.0 * model->i_frequency * std::numbers::pi * 2.0)
    });
    m_o_ready.push_back({ getContext()->time(), model->o_ready });
  }

  virtual void onParametrizedStep(Vmod_sinesource* model) override {
    // Firstly, step through two clock cycles with a low trigger.
    m_trigger = 0;
    step(2);

    // Then, step with a high trigger.
    m_trigger = 1;
    step(2);

    // The model should eventually raise o_ready.
    m_trigger = 0;
    stepUntil(std::function([](Vmod_sinesource* model) -> bool { return model->o_ready; }));
  }

  void plotTimeSeries() {
    Plotting::plot({
      &m_i_time, &m_i_frequency, &m_i_trigger, &m_i_clk, &m_i_nrst,
      &m_o_ready, &m_o_sine, &m_o_sine_expected
    });
  }

private:
  bool m_trigger = false;

  Plotting::Signal<int32_t, uint64_t> m_i_time { "i\\\\_times" };
  Plotting::Signal<int32_t, uint64_t> m_i_frequency { "i\\\\_frequency" };
  Plotting::DigitalSignal<int32_t, bool> m_i_trigger { "i\\\\_trigger" };
  Plotting::DigitalSignal<int32_t, bool> m_i_clk { "i\\\\_clocks" };
  Plotting::DigitalSignal<int32_t, bool> m_i_nrst { "i\\\\_nrst" };

  Plotting::Signal<int32_t, double> m_o_sine { "o\\\\_sine" };
  Plotting::Signal<int32_t, double> m_o_sine_expected { "o\\\\_sine\\\\_expected" };
  Plotting::DigitalSignal<int32_t, bool> m_o_ready { "o\\\\_ready" };
};

TEST_CASE("Test sinesource against Orpheus") {
  const std::unique_ptr<VerilatedContext> ctx { new VerilatedContext };
  TestFixture testFixture { ctx.get(), "test.vcd" };

  testFixture.runParametrizedTests();
  testFixture.plotTimeSeries();
}
