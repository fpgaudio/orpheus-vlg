#include "orpheus.hpp"
#include "test/model_tester.hpp"
#include "test/test_utils.hpp"
#include "verilated.h"
#include <cstdint>
#include <fstream>
#include <limits>
#include <memory>
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include "Vmod_attenuator.h"
#include "orpheus_util.hpp"

static std::string outputDirectory;

class TestFixture : public Svt::AbstractTest<Vmod_attenuator, true>
{
public:
  TestFixture(VerilatedContext* ctx, const char* vcdFileName)
    : Svt::AbstractTest<Vmod_attenuator, true>(ctx, vcdFileName) {}

  virtual void populateInputs(std::shared_ptr<Vmod_attenuator> model) override {
    model->i_raw = getParameter<Svt::I32RangeParam>("raw")->get();
    model->i_attenfactor = getParameter<Svt::I32RangeParam>("attenfactor")->get();
  }

  virtual void seedParameters() override {
    getParameters().emplace(std::make_pair("raw",
      std::make_unique<Svt::I32RangeParam>(
        Svt::float2fixed<14, int32_t>(1),
        Svt::float2fixed<14, int32_t>(10),
        Svt::float2fixed<14, int32_t>(1)
      )));
    getParameters().emplace(std::make_pair("attenfactor",
      std::make_unique<Svt::I32RangeParam>(
        Svt::float2fixed<14, int32_t>(0.1),
        Svt::float2fixed<14, int32_t>(2),
        Svt::float2fixed<14, int32_t>(0.1)
      )));
  }

  virtual void onStep(std::shared_ptr<Vmod_attenuator> model) override {
    m_i_raw.push_back({ getContext()->time(), model->i_raw });
    m_i_attenfactor.push_back({ getContext()->time(), model->i_attenfactor });
    m_o_attenuated.push_back({ getContext()->time(), model->o_attenuated });
  }

  void plotTimeSeries() {
    std::initializer_list<Plotting::AbstractSignal*> signals = {
      &m_i_raw, &m_i_attenfactor, &m_o_attenuated
    };

    Plotting::plot(signals, Plotting::PlotMode::PNG,
                   artifactPath(outputDirectory, "test_mod_attenuator_plots.png"));

    for (auto sig : signals) {
      std::ofstream dumpFile {
        artifactPath(outputDirectory, "test_mod_attenuator_" + sig->getSignalName() + ".txt")
      };
      sig->dump(dumpFile);
    }
  }

private:
  Plotting::Signal<uint32_t, int32_t> m_o_attenuated { "o_attenuated" };
  Plotting::Signal<uint32_t, int32_t> m_i_raw { "i_raw" };
  Plotting::Signal<uint32_t, int16_t> m_i_attenfactor { "i_attenfactor" };
};

TEST_CASE("Test attenutator against Orpheus") {
  const std::unique_ptr<VerilatedContext> ctx { new VerilatedContext };
  TestFixture testFixture { ctx.get(), "test.vcd" };

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
