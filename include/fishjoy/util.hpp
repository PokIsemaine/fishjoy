//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_UTIL_HPP
#define FISHJOY_UTIL_HPP

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>

namespace fishjoy
{
  pid_t GetThreadId();
  uint32_t GetFiberId();

  void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);
  std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");

}  // namespace fishjoy

#endif  // FISHJOY_UTIL_HPP
