//
// Created by zsl on 6/1/22.
//

#ifndef FISHJOY_FIBER_HPP
#define FISHJOY_FIBER_HPP

#include <memory>
#include <functional>
#include <ucontext.h>
#include "mutex.hpp"
#include "thread.hpp"

namespace fishjoy {
class Fiber: public std::enable_shared_from_this<Fiber> {
 public:
  using ptr = std::shared_ptr<Fiber>;

  enum State {
    INIT,
    HOLD,
    EXEC,
    TERM,
    READY,
    EXCEPT
  };

 public:
  Fiber(std::function<void()> callback, size_t stacksize = 0);
  ~Fiber();

 private:
  Fiber();

  //重置协程函数，并重置状态
  //INIT,TERM
  void reset(std::function<void()> callback);

  //切换到当前协程执行
  void swapIn();

  //切换到后台执行
  void swapOut();

  void call();

  uint64_t getId() const { return  m_id;}
  State getState() const { return m_state;}

  //设置当前协程
  static void SetThis(Fiber* f);

  //返回当前协程
  static Fiber::ptr GetThis();

  //协程切换到后台，并设置为Ready 状态
  static void YieldToReady();

  //总协程数
  static uint64_t TotalFibers();

  static void MainFunc();
  static uint64_t GetFiberId();

 private:
  uint64_t m_id = 0;
  uint32_t m_stacksize = 0;
  State m_state = INIT;

  ucontext_t m_ctx;
  void* m_stack = nullptr;

  std::function<void()> m_callback;
};
}

#endif  // FISHJOY_FIBER_HPP
