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

namespace fishjoy
{

  pid_t GetThreadId();
  uint32_t GetFiberId();
}  // namespace fishjoy
#endif  // FISHJOY_UTIL_HPP
