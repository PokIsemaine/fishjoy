//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_UTIL_HPP
#define FISHJOY_UTIL_HPP

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <vector>
#include <cxxabi.h> // for abi::__cxa_demangle()

namespace fishjoy
{
  /**
 * @brief 获取线程id
 * @note 这里不要把pid_t和pthread_t混淆，关于它们之的区别可参考getting(2)
   */
  pid_t GetThreadId();

  /**
 * @brief 获取协程id
 * @todo 桩函数，暂时返回0，等协程模块完善后再返回实际值
   */
  uint64_t GetFiberId();

  /**
 * @brief 获取当前启动的毫秒数，参考clock_getting(2)，使用CLOCK_MONOTONIC_RAW
   */
  uint64_t GetElapsedMS();

  /**
 * @brief 获取线程名称，参考pthread_get-name_np(3)
   */
  std::string GetThreadName();

  /**
 * @brief 设置线程名称，参考pthread_set name_np(3)
 * @note 线程名称不能超过16字节，包括结尾的'\0'字符
   */
  void SetThreadName(const std::string &name);

  /**
 * @brief 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
   */
  void Backtrace(std::vector<std::string> &bt, int size = 64, int skip = 1);

  /**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
   */
  std::string BacktraceToString(int size = 64, int skip = 2, const std::string &prefix = "");

  /**
 * @brief 获取当前时间的毫秒
   */
  uint64_t GetCurrentMS();

  /**
 * @brief 获取当前时间的微秒
   */
  uint64_t GetCurrentUS();

  /**
 * @brief 字符串转大写
   */
  std::string ToUpper(const std::string &name);

  /**
 * @brief 字符串转小写
   */
  std::string ToLower(const std::string &name);

  /**
 * @brief 日期时间转字符串
   */
  std::string Time2Str(time_t ts = time(0), const std::string &format = "%Y-%m-%d %H:%M:%S");

  /**
 * @brief 字符串转日期时间
   */
  time_t Str2Time(const char *str, const char *format = "%Y-%m-%d %H:%M:%S");


  /**
 * @brief 类型转换
   */
  class TypeUtil {
   public:
    /// 转字符，返回*str.begin()
    static int8_t ToChar(const std::string &str);
    /// atoi，参考atoi(3)
    static int64_t Atoi(const std::string &str);
    /// atof，参考atof(3)
    static double Atof(const std::string &str);
    /// 返回str[0]
    static int8_t ToChar(const char *str);
    /// atoi，参考atoi(3)
    static int64_t Atoi(const char *str);
    /// atof，参考atof(3)
    static double Atof(const char *str);
  };

  /**
 * @brief 获取T类型的类型字符串
   */
  template <class T>
  const char *TypeToName() {
    static const char *s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
  }


  /**
 * @brief 字符串辅助类
   */
  class StringUtil {
   public:
    /**
     * @brief printf风格的字符串格式化，返回格式化后的string
     */
    static std::string Format(const char* fmt, ...);

    /**
     * @brief vprintf风格的字符串格式化，返回格式化后的string
     */
    static std::string Formatv(const char* fmt, va_list ap);

    /**
     * @brief url编码
     * @param[in] str 原始字符串
     * @param[in] space_as_plus 是否将空格编码成+号，如果为false，则空格编码成%20
     * @return 编码后的字符串
     */
    static std::string UrlEncode(const std::string& str, bool space_as_plus = true);

    /**
     * @brief url解码
     * @param[in] str url字符串
     * @param[in] space_as_plus 是否将+号解码为空格
     * @return 解析后的字符串
     */
    static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

    /**
     * @brief 移除字符串首尾的指定字符串
     * @param[] str 输入字符串
     * @param[] delimit 待移除的字符串
     * @return  移除后的字符串
     */
    static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");

    /**
     * @brief 移除字符串首部的指定字符串
     * @param[] str 输入字符串
     * @param[] delimit 待移除的字符串
     * @return  移除后的字符串
     */
    static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");

    /**
     * @brief 移除字符尾部的指定字符串
     * @param[] str 输入字符串
     * @param[] delimit 待移除的字符串
     * @return  移除后的字符串
     */
    static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");

    /**
     * @brief 宽字符串转字符串
     */
    static std::string WStringToString(const std::wstring& ws);

    /**
     * @brief 字符串转宽字符串
     */
    static std::wstring StringToWString(const std::string& s);

  };

}  // namespace fishjoy

#endif  // FISHJOY_UTIL_HPP
