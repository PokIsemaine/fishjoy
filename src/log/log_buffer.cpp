//
// Created by zsl on 8/24/22.
//

#include "fishjoy/log/log_buffer.hpp"
#include <cstring>

namespace fishjoy {
  LogBuffer::LogBuffer(size_t total) : m_total(total), m_available(total), m_cur(0) {
    m_data = new char[total];
    setCookie(cookieStart);
  }

  LogBuffer::~LogBuffer() {
    delete[] m_data;
    setCookie(cookieEnd);
  }

  size_t LogBuffer::available() const { return m_available; }

  void LogBuffer::clear() {
    m_cur = 0;
    m_available = m_total;
  }

  void LogBuffer::append(const char *data, size_t len) {
    memcpy(m_data + m_cur, data, len);
    m_cur += len;
    m_available -= len;
  }

  const char *LogBuffer::data() const { return m_data; }

  size_t LogBuffer::length() const { return m_cur; }

  void LogBuffer::cookieStart() {}

  void LogBuffer::cookieEnd() {}
}
