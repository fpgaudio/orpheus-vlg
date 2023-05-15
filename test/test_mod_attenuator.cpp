#include "orpheus.hpp"
#include "test/model_tester.hpp"
#include "test/test_utils.hpp"
#include "verilated.h"
#include <cstdint>
#include <fstream>
#include <limits>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "Vmod_attenuator.h"
#include "orpheus_util.hpp"

class TestFixture : public Svt::AbstractTest<Vmod_attenuator, true>
{
public:
  TestFixture(VerilatedContext* ctx, const char* vcdFileName)
    : Svt::AbstractTest<Vmod_attenuator, true>(ctx, vcdFileName) {}

  virtual void populateInputs(Vmod_attenuator* model) override {
  }

  virtual void seedParameters() override {
  }

  virtual void onStep() {
  }
}

TEST_CASE("Test attenutator against Orpheus") {
  const std::unique_ptr<VerilatedContext> ctx { new VerilatedContext };
  TestFixture testFixture { ctx.get(), "test.vcd" };

  testFixture.runParametrizedTests();
  testFixture.plotTimeSeries();
}
