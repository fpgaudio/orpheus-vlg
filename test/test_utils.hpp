#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
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
    auto& canvas = *eng;
    canvas << "set grid" << std::endl;
    canvas << "plot '-' with lines title \"" << m_signalName << "\"" << std::endl;
    canvas.send1d(m_data);
#endif // ORPHEUS_VLG_TEST_PLOTTING
  }

  virtual void push_back(std::tuple<T, K> datum) {
    m_data.push_back(datum);
  }

  constexpr std::string& getSignalName() { return m_signalName; }
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
    auto& canvas = *eng;
    canvas << "set grid" << std::endl;
    canvas << "set yrange [-0.1:1.1]" << std::endl;
    canvas << "set ytics 0,1,1" << std::endl;
    canvas << "plot '-' with steps title \"" << this->getSignalName() << "\"" << std::endl;
    canvas.send1d(this->getData());
#endif // ORPHEUS_VLG_TEST_PLOTTING
  }
};

inline void plot(std::initializer_list<AbstractSignal*> signals) {
#ifdef ORPHEUS_VLG_TEST_PLOTTING
  gnuplotio::Gnuplot canvas;
  canvas << "set multiplot layout " << signals.size() << ", 1" << std::endl;
  for (auto signal : signals) {
    canvas << "reset" << std::endl;
    canvas << "set lmargin 10" << std::endl;
    signal->plot(&canvas);
  }
#else
  std::cerr << "Warning: Plots are not currently built. No graphs will be output." << std::endl;
  std::cerr << "  To enable graphs, run `meson configure --Dtest-graphs=enabled" << std::endl;
#endif
}

} // namespace Plotting

#endif // TEST_UTILS_HPP
