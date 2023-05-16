#include "test/model_tester.hpp"
#include "test/test_utils.hpp"
#include "verilated.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <limits>
#include <memory>
#include <utility>
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include "Vmod_synth.h"
#include "orpheus_util.hpp"
#include <variant>

static std::string outputDirectory;

class TestFixture : public Svt::AbstractTest<Vmod_synth, true>
{
public:
  TestFixture(VerilatedContext* ctx, const char* vcdFileName)
    : Svt::AbstractTest<Vmod_synth, true>(ctx, vcdFileName) {}

  virtual void populateInputs(std::shared_ptr<Vmod_synth> model) override {
    model->i_time = getParameter<Svt::U64RangeParam>("time")->get();
    model->i_atten_harmonics[0] = getParameter<Svt::U32RangeParam>("atten_harmonic_0")->get();
    model->i_atten_harmonics[1] = getParameter<Svt::U32RangeParam>("atten_harmonic_1")->get();
    model->i_atten_harmonics[2] = getParameter<Svt::U32RangeParam>("atten_harmonic_2")->get();
    model->i_atten_harmonics[3] = getParameter<Svt::U32RangeParam>("atten_harmonic_3")->get();
    model->i_atten_harmonics[4] = getParameter<Svt::U32RangeParam>("atten_harmonic_4")->get();
    model->i_atten_out = getParameter<Svt::U32RangeParam>("atten_out")->get();
    model->i_frequency = getParameter<Svt::U32RangeParam>("frequency")->get();
    model->i_nrst = getParameter<Svt::U32RangeParam>("nrst")->get();
    model->i_trigger = m_trigger;
    model->i_clk = getClock().getState();
  }

  virtual void onParametrizedStep(std::shared_ptr<Vmod_synth> model) override {
    // Firstly, step through two clock cycles with a low trigger.
    m_trigger = 0;
    step(2);

    // Then, step with a high trigger.
    m_trigger = 1;
    step(2);

    // The model should eventually raise o_ready.
    m_trigger = 0;
    stepUntil(std::function([](std::shared_ptr<Vmod_synth> model) -> bool {
      return model->o_ready;
    }));
  }

  virtual void seedParameters() override {
    getParameters().emplace(std::make_pair("time",
      std::make_unique<Svt::U64RangeParam>(0, 200, 1)));

    // Seed the harmonics
    for (std::size_t i = 0 ; i < 5; i++) {
      getParameters().emplace(std::make_pair(
        "atten_harmonic_" + std::to_string(i),
        std::make_unique<Svt::U32RangeParam>(
          Svt::float2fixed<14, int32_t>(1.0 / (i + 1.0)),
          Svt::float2fixed<14, int32_t>(1.0 / (i + 1.0)),
          Svt::float2fixed<14, int32_t>(0.5)
      )));
    }
    getParameters().emplace(std::make_pair("atten_out", std::make_unique<Svt::U32RangeParam>(
      Svt::float2fixed<14, int32_t>(1),
      Svt::float2fixed<14, int32_t>(1),
      Svt::float2fixed<14, int32_t>(0.5)
    )));

    getParameters().emplace(std::make_pair("frequency",
      std::make_unique<Svt::U32Param>(440)));
    getParameters().emplace(std::make_pair("nrst", std::make_unique<Svt::U32RangeParam>(1, 1)));
  }

  virtual void onStep(std::shared_ptr<Vmod_synth> model) override {
    m_i_frequency.push_back({ getContext()->time(), model->i_frequency });
    m_i_trigger.push_back({ getContext()->time(), model->i_trigger });
    m_i_clk.push_back({ getContext()->time(), model->i_clk });
    m_i_nrst.push_back({ getContext()->time(), model->i_nrst });
    m_i_atten_harmonic_0.push_back({ getContext()->time(), model->i_atten_harmonics[0] });
    m_i_atten_harmonic_1.push_back({ getContext()->time(), model->i_atten_harmonics[1] });
    m_i_atten_harmonic_2.push_back({ getContext()->time(), model->i_atten_harmonics[2] });
    m_i_atten_harmonic_3.push_back({ getContext()->time(), model->i_atten_harmonics[3] });
    m_i_atten_harmonic_4.push_back({ getContext()->time(), model->i_atten_harmonics[4] });
    m_i_atten_out.push_back({ getContext()->time(), model->i_atten_out });
    m_i_time.push_back({ getContext()->time(), model->i_time });
    m_o_ready.push_back({ getContext()->time(), model->o_ready });
    m_o_sound.push_back({ getContext()->time(), model->o_sound });
  }

  void plotTimeSeries() {
    std::initializer_list<Plotting::AbstractSignal*> signals = {
      &m_i_time, &m_i_frequency, &m_i_trigger, &m_i_clk, &m_i_nrst,
      &m_i_atten_harmonic_0, &m_i_atten_harmonic_1, &m_i_atten_harmonic_2,
      &m_i_atten_harmonic_3, &m_i_atten_harmonic_4, &m_i_atten_out,
      &m_o_ready, &m_o_sound
    };
    Plotting::plot(signals, Plotting::PlotMode::PNG,
      artifactPath(outputDirectory, "test_mod_synth_plots.png"));

    for (auto sig : signals) {
      std::ofstream dumpFile {
        artifactPath(outputDirectory, "test_mod_synth_" + sig->getSignalName() + ".txt")
      };
      sig->dump(dumpFile);
    }
  }

private:
  bool m_trigger = false;

  Plotting::Signal<int32_t, uint64_t> m_i_time { "i_times" };
  Plotting::Signal<int32_t, uint64_t> m_i_frequency { "i_frequency" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_harmonic_0 { "i_atten_harmonic_0" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_harmonic_1 { "i_atten_harmonic_1" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_harmonic_2 { "i_atten_harmonic_2" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_harmonic_3 { "i_atten_harmonic_3" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_harmonic_4 { "i_atten_harmonic_4" };
  Plotting::Signal<int32_t, int32_t> m_i_atten_out { "i_atten_out" };
  Plotting::DigitalSignal<int32_t, bool> m_i_trigger { "i_trigger" };
  Plotting::DigitalSignal<int32_t, bool> m_i_clk { "i_clocks" };
  Plotting::DigitalSignal<int32_t, bool> m_i_nrst { "i_nrst" };
  Plotting::DigitalSignal<int32_t, bool> m_o_ready { "o_ready" };
  Plotting::Signal<int32_t, int32_t> m_o_sound { "o_sound" };
};

TEST_CASE("Test synthesizer") {
  const std::unique_ptr<VerilatedContext> ctx { new VerilatedContext };
  TestFixture testFixture {
    ctx.get(),
    artifactPath(outputDirectory, "test_mod_synth.vcd").c_str()
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
