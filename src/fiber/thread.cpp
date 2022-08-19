//
// Created by zsl on 5/29/22.
//

#include "fishjoy/fiber/thread.hpp"

#include "fishjoy/fiber/mutex.hpp"
#include "fishjoy/log/log.hpp"
#include "fishjoy/utils/util.hpp"

namespace fishjoy {
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  static thread_local Thread* t_thread = nullptr;
  static thread_local std::string t_thread_name = "UNKNOWN";

  Thread* Thread::GetThis() { return t_thread; }

  const std::string& Thread::GetName() { return t_thread_name; }

  void Thread::SetName(const std::string& name) {
    if (t_thread != nullptr) {
      t_thread->m_name = name;
    }
    t_thread_name = name;
  }

  Thread::Thread(std::function<void()> callback, const std::string& name) : m_callback(callback), m_name(name) {
    if (name.empty()) {
      m_name = "UNKNOWN";
    }
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (ret != 0) {
      FISHJOY_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << ret << " name=" << name;
      throw std::logic_error("pthread_create error");
    }
    // 通过一个信号量来保证在构造完成之后线程函数一定已经处于运行状态
    // 构造函数在创建线程后会一直阻塞，直到线程函数运行并且通知信号量
    // 构造函数才会返回，而构造函数一旦返回，就说明线程函数已经在执行了
    m_semaphore.wait();
  }

  Thread::~Thread() {
    if (m_thread != 0U) {
      pthread_detach(m_thread);
    }
  }

  void Thread::join() {
    if (m_thread) {
      auto ret = pthread_join(m_thread, nullptr);
      if (ret != 0) {
        FISHJOY_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << ret << " name=" << m_name;
        throw std::logic_error("pthread_join error");
      }
      m_thread = 0;
    }
  }

  void* Thread::run(void* arg) {
    auto* thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = fishjoy::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> callback;
    callback.swap(thread->m_callback);
    // 通知构造函数线程函数运行起来了
    thread->m_semaphore.notify();
    // 执行线程函数
    callback();

    return nullptr;
  }

}  // namespace fishjoy
