//
// Created by zsl on 6/1/22.
//

#include "fishjoy/fiber.hpp"
#include "fishjoy/config.hpp"
#include "fishjoy/macro.hpp"
#include "fishjoy/log.hpp"

#include "atomic"

namespace fishjoy {

  static Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  static std::atomic<uint64_t > s_fiber_id {0};
  static std::atomic<uint64_t > s_fiber_count {0};

  static thread_local Fiber* t_fiber = nullptr;
  static thread_local Fiber::ptr t_threadFiber = nullptr;

  static ConfigVar<uint32_t >::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t >("fiber.stack_size", 1024*1024,"fiber stack size");

  class MallocStackAllocator {
   public:
    static void* Alloc(size_t size) {
      return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
      return free(vp);
    }
  };

  using StackAllocator = MallocStackAllocator;

  uint64_t Fiber::GetFiberId() {
    return t_fiber ? t_fiber->getId() : 0;
  }

  Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
      FISHJOY_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    FISHJOY_LOG_DEBUG(g_logger) << "Fiber::Fiber";
  }

  Fiber::Fiber(std::function<void()> callback, size_t stacksize)
    :m_id(++s_fiber_id)
    ,m_callback(callback) {
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)) {
      FISHJOY_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    FISHJOY_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
  }

  Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
      FISHJOY_ASSERT(m_state == TERM
                     || m_state == EXCEPT
                     || m_state == INIT);

      StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
      FISHJOY_ASSERT(!m_callback);
      FISHJOY_ASSERT(m_state == EXEC);

      Fiber* cur = t_fiber;
      if(cur == this) {
        SetThis(nullptr);
      }
    }
    FISHJOY_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
  }

  //重置协程函数，并重置状态
  //INIT，TERM
  void Fiber::reset(std::function<void()> callback) {
    FISHJOY_ASSERT(m_stack);
    FISHJOY_ASSERT(m_state == TERM
                   || m_state == EXCEPT
                   || m_state == INIT);
    m_callback = callback;
    if(getcontext(&m_ctx)) {
      FISHJOY_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;

  }

  void Fiber::call() {
    m_state = EXEC;
    FISHJOY_LOG_ERROR(g_logger) << getId();
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
      FISHJOY_ASSERT2(false, "swapcontext");
    }
  }

  //切换到当前协程执行
  void Fiber::swapIn() {
    SetThis(this);
    FISHJOY_ASSERT(m_state != EXEC);
    m_state = EXEC;

  }
  //切换到后台执行
  void Fiber::swapOut() {

  }
  //设置当前协程
  void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
  }
  //返回当前协程
  static Fiber::ptr GetThis() {

  }
  //协程切换到后台，并且设置为Ready状态
  static void YieldToReady() {

  }
  //协程切换到后台，并且设置为Hold状态
  static void YieldToHold() {

  }
  //总协程数
  static uint64_t TotalFibers() {

  }
  static void MainFunc() {

  }
}