#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

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

#endif // TEST_UTILS_HPP
