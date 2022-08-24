//
// Created by zsl on 8/23/22.
//

#ifndef FISHJOY_LOG_BUFFER_HPP
#define FISHJOY_LOG_BUFFER_HPP

#include <sys/types.h>

namespace fishjoy {
  class LogBuffer {
   public:
    LogBuffer(size_t total = 1024 * 1024 * 10);
    ~LogBuffer();

    void clear();
    void append(const char *data, size_t len);
    const char *data() const;
    size_t length() const;
    size_t available() const;
    // for used by GDB
    const char *debugString();
    void setCookie(void (*cookie)()) { m_cookie = cookie; }

   private:
    char * m_data;
    const size_t m_total;
    size_t m_available;
    size_t m_cur;
    // Must be outline function for cookies.
    static void cookieStart();
    static void cookieEnd();

    void (*m_cookie)();
  };
}

#endif  // FISHJOY_LOG_BUFFER_HPP
