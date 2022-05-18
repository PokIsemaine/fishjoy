//
// Created by zsl on 5/18/22.
//

#include "fishjoy/util.hpp"

namespace fishjoy
{
  pid_t GetThreadId()
  {
    return syscall(SYS_gettid);
  }

  uint32_t GetFiberId()
  {
    return 0;
  }
}  // namespace fishjoy