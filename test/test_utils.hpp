#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <fstream>
#include <iomanip>
#include <string>
#include <string_view>

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

  template<template <typename> class T, typename Elem>
  void dump(T<Elem> data) {
    for (auto datum : data) { dump(datum); }
  }

private:
  const std::string m_filepath;
  std::ofstream m_ofstream;
};

#endif // TEST_UTILS_HPP
