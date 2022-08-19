//
// Created by zsl on 5/18/22.
//

#include "fishjoy/utils/util.hpp"

#include <cxxabi.h>    // for abi::__cxa_demangle()
#include <execinfo.h>  // for backtrace()
#include <string.h>

#include <algorithm>  // for std::transform()

#include "fishjoy/fiber/fiber.hpp"
#include "fishjoy/log/log.hpp"

namespace fishjoy {
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");
  
  pid_t GetThreadId() {
    return syscall(SYS_gettid);
  }

  uint64_t GetFiberId() {
    return Fiber::GetFiberId();
  }

  uint64_t GetElapsedMS() {
    struct timespec ts = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  }

  std::string GetThreadName() {
    char thread_name[16] = {0};
    pthread_getname_np(pthread_self(), thread_name, 16);
    return std::string(thread_name);
  }

  void SetThreadName(const std::string &name) {
    pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
  }

  static std::string demangle(const char *str) {
    size_t size = 0;
    int status  = 0;
    std::string rt;
    rt.resize(256);
    if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) {
      char *v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
      if (v) {
        std::string result(v);
        free(v);
        return result;
      }
    }
    if (1 == sscanf(str, "%255s", &rt[0])) {
      return rt;
    }
    return str;
  }

  void Backtrace(std::vector<std::string> &bt, int size, int skip) {
    void **array = (void **)malloc((sizeof(void *) * size));
    size_t s     = ::backtrace(array, size);

    char **strings = backtrace_symbols(array, s);
    if (strings == NULL) {
      FISHJOY_LOG_ERROR(g_logger) << "backtrace_synbols error";
      return;
    }

    for (size_t i = skip; i < s; ++i) {
      bt.push_back(demangle(strings[i]));
    }

    free(strings);
    free(array);
  }

  std::string BacktraceToString(int size, int skip, const std::string &prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
      ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
  }

  uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
  }

  uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
  }

  std::string ToUpper(const std::string &name) {
    std::string rt = name;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::toupper);
    return rt;
  }

  std::string ToLower(const std::string &name) {
    std::string rt = name;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::tolower);
    return rt;
  }

  std::string Time2Str(time_t ts, const std::string &format) {
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
  }

  time_t Str2Time(const char *str, const char *format) {
    struct tm t;
    memset(&t, 0, sizeof(t));
    if (!strptime(str, format, &t)) {
      return 0;
    }
    return mktime(&t);
  }

  int8_t TypeUtil::ToChar(const std::string &str) {
    if (str.empty()) {
      return 0;
    }
    return *str.begin();
  }

  int64_t TypeUtil::Atoi(const std::string &str) {
    if (str.empty()) {
      return 0;
    }
    return strtoull(str.c_str(), nullptr, 10);
  }

  double TypeUtil::Atof(const std::string &str) {
    if (str.empty()) {
      return 0;
    }
    return atof(str.c_str());
  }

  int8_t TypeUtil::ToChar(const char *str) {
    if (str == nullptr) {
      return 0;
    }
    return str[0];
  }

  int64_t TypeUtil::Atoi(const char *str) {
    if (str == nullptr) {
      return 0;
    }
    return strtoull(str, nullptr, 10);
  }

  double TypeUtil::Atof(const char *str) {
    if (str == nullptr) {
      return 0;
    }
    return atof(str);
  }


  static const char uri_chars[256] = {
    /* 0 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 1, 0, 0,
    /* 64 */
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 1, 0,
    /* 128 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    /* 192 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
  };

  static const char xdigit_chars[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

#define CHAR_IS_UNRESERVED(c)           \
    (uri_chars[(unsigned char)(c)])

  //-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~
  std::string StringUtil::UrlEncode(const std::string& str, bool space_as_plus) {
    static const char *hexdigits = "0123456789ABCDEF";
    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for(const char* c = str.c_str() ; c < end; ++c) {
      if(!CHAR_IS_UNRESERVED(*c)) {
        if(!ss) {
          ss = new std::string;
          ss->reserve(str.size() * 1.2);
          ss->append(str.c_str(), c - str.c_str());
        }
        if(*c == ' ' && space_as_plus) {
          ss->append(1, '+');
        } else {
          ss->append(1, '%');
          ss->append(1, hexdigits[(uint8_t)*c >> 4]);
          ss->append(1, hexdigits[*c & 0xf]);
        }
      } else if(ss) {
        ss->append(1, *c);
      }
    }
    if(!ss) {
      return str;
    } else {
      std::string rt = *ss;
      delete ss;
      return rt;
    }
  }

  std::string StringUtil::UrlDecode(const std::string& str, bool space_as_plus) {
    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for(const char* c = str.c_str(); c < end; ++c) {
      if(*c == '+' && space_as_plus) {
        if(!ss) {
          ss = new std::string;
          ss->append(str.c_str(), c - str.c_str());
        }
        ss->append(1, ' ');
      } else if(*c == '%' && (c + 2) < end
                 && isxdigit(*(c + 1)) && isxdigit(*(c + 2))){
        if(!ss) {
          ss = new std::string;
          ss->append(str.c_str(), c - str.c_str());
        }
        ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
        c += 2;
      } else if(ss) {
        ss->append(1, *c);
      }
    }
    if(!ss) {
      return str;
    } else {
      std::string rt = *ss;
      delete ss;
      return rt;
    }
  }

  std::string StringUtil::Trim(const std::string& str, const std::string& delimit) {
    auto begin = str.find_first_not_of(delimit);
    if(begin == std::string::npos) {
      return "";
    }
    auto end = str.find_last_not_of(delimit);
    return str.substr(begin, end - begin + 1);
  }

  std::string StringUtil::TrimLeft(const std::string& str, const std::string& delimit) {
    auto begin = str.find_first_not_of(delimit);
    if(begin == std::string::npos) {
      return "";
    }
    return str.substr(begin);
  }

  std::string StringUtil::TrimRight(const std::string& str, const std::string& delimit) {
    auto end = str.find_last_not_of(delimit);
    if(end == std::string::npos) {
      return "";
    }
    return str.substr(0, end);
  }

  std::string StringUtil::WStringToString(const std::wstring& ws) {
    std::string str_locale = setlocale(LC_ALL, "");
    const wchar_t* wch_src = ws.c_str();
    size_t n_dest_size = wcstombs(NULL, wch_src, 0) + 1;
    char *ch_dest = new char[n_dest_size];
    memset(ch_dest,0,n_dest_size);
    wcstombs(ch_dest,wch_src,n_dest_size);
    std::string str_result = ch_dest;
    delete []ch_dest;
    setlocale(LC_ALL, str_locale.c_str());
    return str_result;
  }

  std::wstring StringUtil::StringToWString(const std::string& s) {
    std::string str_locale = setlocale(LC_ALL, "");
    const char* chSrc = s.c_str();
    size_t n_dest_size = mbstowcs(NULL, chSrc, 0) + 1;
    wchar_t* wch_dest = new wchar_t[n_dest_size];
    wmemset(wch_dest, 0, n_dest_size);
    mbstowcs(wch_dest,chSrc,n_dest_size);
    std::wstring wstr_result = wch_dest;
    delete []wch_dest;
    setlocale(LC_ALL, str_locale.c_str());
    return wstr_result;
  }

}  // namespace fishjoy