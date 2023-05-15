#ifndef MODEL_TESTER_HPP
#define MODEL_TESTER_HPP

#include "verilated.h"
#include "verilated_vcd_c.h"
#include <cstdint>
#include <iostream>
#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace Svt
{

template<std::size_t mantissaWidth, typename T, typename FloatLike>
T float2fixed(FloatLike val) {
  static_assert(
    mantissaWidth <= std::numeric_limits<FloatLike>::digits,
    "The given type does not support the specified mantissa width. Seems like an error."
  );
  return static_cast<T>(std::round(val * (1 << mantissaWidth)));
}

template<std::size_t mantissaWidth, typename FloatLike, typename T>
FloatLike fixed2float(T val) {
  static_assert(
    mantissaWidth <= std::numeric_limits<FloatLike>::digits,
    "The given type does not support the specified mantissa width. Seems like an error."
  );

  return static_cast<FloatLike>(val) / std::pow(2, mantissaWidth);
}

class ClockProvider
{
public:
  inline void toggleUntil(std::function<bool (bool)> predicate, size_t maxCycles = 10000) {
    // A toggle is only half a cycle --v
    for (std::size_t i = 0; i < maxCycles * 2; i++, toggle()) {
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

class AbstractTestParameter
{
public:
  virtual void step() = 0;
  virtual bool end() = 0;
};

template <typename T>
class RangeParam : public AbstractTestParameter
{
public:
  RangeParam(T start, T stop, T gait = 1): m_start(start), m_stop(stop), m_gait(gait) {}
  constexpr void step() override { m_value += m_gait; }
  constexpr bool end() override { return m_value == m_stop; }
  constexpr T get() { return m_value; }

private:
  T m_start;
  T m_stop;
  T m_gait;
  T m_value = m_start;
};

using I32RangeParam = RangeParam<std::int32_t>;
struct I32Param : public I32RangeParam { I32Param(std::int32_t v) : I32RangeParam(v, v, v) {} };
using U32RangeParam = RangeParam<std::uint32_t>;
struct U32Param : public U32RangeParam { U32Param(std::uint32_t v) : U32RangeParam(v, v, v) {} };
using I64RangeParam = RangeParam<std::int64_t>;
struct I64Param : public I64RangeParam { I64Param(std::int64_t v) : I64RangeParam(v, v, v) {} };
using U64RangeParam = RangeParam<std::uint64_t>;
struct U64Param : public U64RangeParam { U64Param(std::uint64_t v) : U64RangeParam(v, v, v) {} };
using BoolRangeParam = RangeParam<bool>;
struct BoolParam : public BoolRangeParam { BoolParam() : BoolRangeParam(false, true) {} };
struct FalseParam : public BoolRangeParam { FalseParam() : BoolRangeParam(false, false) {} };
struct TrueParam : public BoolRangeParam { TrueParam() : BoolRangeParam(true, true) {} };

template <typename Model, bool WithTracing>
class AbstractTest
{
public:
  AbstractTest() {
    static_assert(!WithTracing, "The AbstractTest() constructor does not support tracing.");
    m_model = std::make_unique<Model>();
  }

  AbstractTest(VerilatedContext* ctx, const char* vcdFileName) : m_vctx(ctx) {
    static_assert(WithTracing, "The AbstractTest(const char*) constructor only supports tracing.");
    Verilated::traceEverOn(true);
    m_vcdFile = std::unique_ptr<VerilatedVcdC> { new VerilatedVcdC };

    m_model = std::make_unique<Model>();

    m_model->trace(m_vcdFile.get(), 99);
    m_vcdFile->open(vcdFileName);
  }

  ~AbstractTest() {
    if constexpr (WithTracing) {
      if (m_vcdFile != nullptr) {
        m_vcdFile->close();
      }
    }
  }

  /**
   * Used to populate model inputs for parametrized testing.
   *
   * Called before each parametrized test step.
   */
  virtual void populateInputs(Model* model) = 0;

  virtual void seedParameters() = 0;

  /**
   * Called before a parametrized step is called. This function is called every time
   * parametrizations change.
   */
  virtual void onBeforeParameterizedStep(Model* model) {}

  /**
   * Called after a parametrized step is called. This function is called every time
   * parametrizations change.
   */
  virtual void onAfterParametrizedStep(Model* model) {}

  /**
   * Called every time a parametrized step is called. By default, this function just steps the
   * model.
   *
   * If you require different behavior, override. By overriding this function, you can run multiple
   * steps within a single parametrization.
   */
  virtual void onParametrizedStep(Model* model) {
    step(model);
  }

  /**
   * Called whenever a model step is evaluated.
   */
  virtual void onStep(Model* model) {}

  /**
   * Does a singular step in the parametrized test.
   *
   * Effectively, this function will firstly call populateInputs(), evaluate and trace the model.
   */
  void step(Model* model) {
    // First, populate inputs.
    populateInputs(model);

    // Evaluate the model.
    model->eval();

    onStep(model);

    // Dump the traces.
    if constexpr (WithTracing) {
      m_vcdFile->dump(m_vctx->time());
      // Time step the context as required.
      m_vctx->timeInc(1);
    }

    // Make a time step
    m_clock.toggle();
  }

  constexpr void step() { step(m_model.get()); }

  void step(Model* model, std::size_t count) {
    for (std::size_t i = 0; i < count; i++) { step(model); }
  }

  constexpr void step(std::size_t count) { step(m_model.get()); }

  /**
   * Steps the simulation until the given predicate returns true.
   *
   * @param predicate Upon returning true, the model stops stepping.
   * @param maxSteps The maximum number of steps the simulation can take before throwing a warning.
   */
  void stepUntil(std::function<bool (Model*)> predicate, std::size_t maxSteps = 1000) {
    for (std::size_t i = 0; i < maxSteps; i++) {
      if (predicate(m_model.get())) {
        return;
      }
      step();
    }

    std::cerr << "[WARNING]: TestFixture ran " << maxSteps << " steps without asserting true "
              << "on the predicate. This looks like an error."
              << std::endl;
  };

  /**
   * Traverses through the parametrized ranges, evaluating the model at every step.
   */
  void runParametrizedTests() {
    // Pull in the child parametrization.
    seedParameters();

    for (const auto& [key, value] : m_parameters) {
      while (!value->end()) {
        onBeforeParameterizedStep(m_model.get());
        onParametrizedStep(m_model.get());
        value->step();
        onAfterParametrizedStep(m_model.get());
      }
    }
  }

  template<typename T>
  T* getParameter(std::string param) {
    return dynamic_cast<T*>(m_parameters.at(param).get());
  };

  constexpr void toggleClock() { m_clock.toggle(); }

protected:
  constexpr auto getClock() -> ClockProvider& { return m_clock; } 
  constexpr auto& getParameters() { return m_parameters; }
  constexpr auto getContext() -> VerilatedContext* {
    static_assert(WithTracing, "This function is unsupported without tracing.");
    return m_vctx;
  }

private:
  ClockProvider m_clock;
  std::unique_ptr<Model> m_model;

  VerilatedContext* m_vctx;
  std::unique_ptr<VerilatedVcdC> m_vcdFile;
  
  std::unordered_map<std::string, std::unique_ptr<AbstractTestParameter>> m_parameters;
};

} // namespace Svt

#endif // MODEL_TESTER_HPP
