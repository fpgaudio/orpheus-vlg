#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <filesystem>
#ifdef ORPHEUS_VLG_TEST_PLOTTING
#define GNUPLOT_ENABLE_PTY
#include <gnuplot-iostream.h>
#endif // ORPHEUS_VLG_TEST_PLOTTING

class LogFile
{
public:
  LogFile() =delete;
  LogFile(const std::string_view filepath) : m_filepath(filepath) {
    m_ofstream.open(m_filepath);
  }
  ~LogFile() {
    m_ofstream.close();
  }

  template<typename T>
  void dump(T datum) {
    m_ofstream
      << std::setw(sizeof(T) * 2) // Each byte requires two characters in hex.
      << std::setfill('0')
      << std::hex
      << datum
      << std::endl;
  }

protected:
  const std::string m_filepath;
  std::ofstream m_ofstream;
};

class CsvLogFile : public LogFile
{
public:
  CsvLogFile(std::string_view filepath,
             const std::vector<std::string_view> columns)
      : LogFile(filepath) {
    for (size_t i = 0; i < columns.size() - 1; i++) {
      m_ofstream << columns[i] << ",";
    }
    m_ofstream << columns[columns.size() - 1] << std::endl;
  }

  template<typename T>
  void dump(std::vector<T> datum) {
    for (size_t i = 0; i < datum.size() - 1; i++) {
      m_ofstream << datum[i] << ",";
    }
    m_ofstream << datum[datum.size() - 1] << std::endl;
  }
};

namespace Plotting
{

#ifdef ORPHEUS_VLG_TEST_PLOTTING
using Engine = gnuplotio::Gnuplot*;
#else
using Engine = void*;
#endif // ORPHEUS_VLG_TEST_PLOTTING

class AbstractSignal
{
public:
  virtual void plot(Engine canvas) = 0;
  virtual void dump(std::ostream& stream) = 0;
  virtual const std::string& getSignalName() = 0;
};

template <typename T, typename K>
class Signal : public AbstractSignal
{
public:
  Signal(std::string_view name) : m_signalName(name) {}

  Signal(std::vector<T> xs, std::vector<K> ys, std::string_view signalName)
    : m_signalName(signalName) {
    if (xs.size() != ys.size()) {
      throw std::runtime_error("Refusing to plot two axes of non-equal lengths.");
    }
    for (size_t i = 0; i < xs.size(); i++) {
      m_data.push_back({ xs.at(i), ys.at(i) });
    }
  }

  virtual void plot(Engine eng) override {
#ifdef ORPHEUS_VLG_TEST_PLOTTING
    const std::string sanitized = std::regex_replace(this->getSignalName(),
                                                     std::regex("_"), "\\\\_");
    auto& canvas = *eng;
    canvas << "set grid" << std::endl;
    canvas << "plot '-' with lines title \"" << sanitized << "\"" << std::endl;
    canvas.send1d(m_data);
#endif // ORPHEUS_VLG_TEST_PLOTTING
  }

  virtual void push_back(std::tuple<T, K> datum) {
    m_data.push_back(datum);
  }

  virtual void dump(std::ostream& into) override {
    for (auto& datum : getData()) {
      into << std::setw(sizeof(K) * 2)
           << std::setfill('0')
           << std::hex
           << std::get<1>(datum)
           << std::endl;
    }
  }

  constexpr virtual std::string& getSignalName() override { return m_signalName; }
  constexpr std::vector<std::tuple<T, K>>& getData() { return m_data; }

protected:
  std::string m_signalName;
  std::vector<std::tuple<T, K>> m_data;
};

template<typename T, typename K>
class DigitalSignal : public Signal<T, K>
{
public:
  DigitalSignal(std::string_view name) : Signal<T, K>(name) {}

  DigitalSignal(std::vector<T> xs, std::vector<K> ys, std::string_view signalName)
    : Signal<T, K>(xs, ys, signalName) {}
  virtual void plot(Engine eng) override {
#ifdef ORPHEUS_VLG_TEST_PLOTTING
    const std::string sanitized = std::regex_replace(this->getSignalName(),
                                                     std::regex("_"), "\\\\_");
    auto& canvas = *eng;
    canvas << "set grid" << std::endl;
    canvas << "set yrange [-0.1:1.1]" << std::endl;
    canvas << "set ytics 0,1,1" << std::endl;
    canvas << "plot '-' with steps title \"" << sanitized << "\"" << std::endl;
    canvas.send1d(this->getData());
#endif // ORPHEUS_VLG_TEST_PLOTTING
  }
};

enum class PlotMode
{
  PNG,
  QT,
  SVG
};

inline void plot(
  std::initializer_list<AbstractSignal*> signals,
  PlotMode plotMode,
  std::string outputName = "plot.png"
) {
#ifdef ORPHEUS_VLG_TEST_PLOTTING
  gnuplotio::Gnuplot canvas;
  // There will be as many axes as are given in the initializer list.
  switch (plotMode)
  {
    case PlotMode::SVG: {
      canvas << "set term svg size 2000, 1200" << std::endl;
      canvas << "set output '" << outputName << "' " << std::endl;
      break;
    }
    case PlotMode::QT: {
      canvas << "set term qt size 2000, 1200" << std::endl;
      break;
    }
    case PlotMode::PNG: {
      canvas << "set term png size 2000, 1200" << std::endl;
      canvas << "set output '" << outputName << "' " << std::endl;
      break;
    }
  }
  canvas << "set multiplot layout " << signals.size() << ", 1" << std::endl;

  // For each value in the initializer list, we need a singular set of axes.
  for (auto signal : signals) {
    // Reset the gnuplot plot on the new axes. Let the individual signal classes decide.
    canvas << "reset" << std::endl;
    canvas << "set lmargin 10" << std::endl;
    canvas << "set bmargin 1.5" << std::endl;
    canvas << "set rmargin 2" << std::endl;
    canvas << "set tmargin 1" << std::endl;

    signal->plot(&canvas);
  }
#else
  std::cerr << "Warning: Plots are not currently built. No graphs will be output." << std::endl;
  std::cerr << "  To enable graphs, run `meson configure --Dtest-graphs=enabled" << std::endl;
#endif
}

} // namespace Plotting

inline std::string artifactPath(std::string outputDirectory, std::string forArtifact) {
  const auto outDir = std::filesystem::absolute(outputDirectory);
  if (!std::filesystem::create_directories(outDir)) {
    if (!std::filesystem::exists(outDir)) {
      throw std::runtime_error(std::string("Cannot create the output directory ")
          + outDir.string() + ". Exiting");
    }
  }
  return outDir / forArtifact;
}

#endif // TEST_UTILS_HPP
