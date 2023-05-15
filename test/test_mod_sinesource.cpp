#include "test/model_tester.hpp"
#include "test/test_utils.hpp"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "Vmod_sinesource.h"
#include <numbers>
#include <filesystem>

static std::string outputDirectory;

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
    std::initializer_list<Plotting::AbstractSignal*> signals = {
      &m_i_time, &m_i_frequency, &m_i_trigger, &m_i_clk, &m_i_nrst,
      &m_o_ready, &m_o_sine, &m_o_sine_expected
    };
    Plotting::plot(signals, Plotting::PlotMode::PNG, artifactPath(outputDirectory,
                   "test_mod_sinesource_plots.png"));

    for (auto sig : signals) {
      std::ofstream dumpFile {
        artifactPath(outputDirectory, "test_mod_sinesource_" + sig->getSignalName() + ".txt")
      };
      sig->dump(dumpFile);
    }
  }

private:
  bool m_trigger = false;

  Plotting::Signal<int32_t, uint64_t> m_i_time { "i_times" };
  Plotting::Signal<int32_t, uint64_t> m_i_frequency { "i_frequency" };
  Plotting::DigitalSignal<int32_t, bool> m_i_trigger { "i_trigger" };
  Plotting::DigitalSignal<int32_t, bool> m_i_clk { "i_clocks" };
  Plotting::DigitalSignal<int32_t, bool> m_i_nrst { "i_nrst" };

  Plotting::Signal<int32_t, double> m_o_sine { "o_sine" };
  Plotting::Signal<int32_t, double> m_o_sine_expected { "o_sine_expected" };
  Plotting::DigitalSignal<int32_t, bool> m_o_ready { "o_ready" };
};

TEST_CASE("Test sinesource against Orpheus") {
  const std::unique_ptr<VerilatedContext> ctx { new VerilatedContext };
  TestFixture testFixture {
    ctx.get(),
    artifactPath(outputDirectory, "test_mod_sinesource.vcd").c_str()
  };

  testFixture.runParametrizedTests();
  testFixture.plotTimeSeries();
}

int main(int argc, char** argv) {
  Catch::Session session;

  // CLI parsing.
  using namespace Catch::clara;
  auto cli
    = session.cli()
    | Opt(outputDirectory, "outputDirectory")
      ["--artifact-directory"]
      ("The output directory for artifacts.")
      .required()
    ;

  // Now pass the new composite back to Catch2 so it uses that
  session.cli(cli);

  auto err = session.applyCommandLine(argc, argv);
  if (err != 0) {
    return err;
  }

  return session.run();
}
