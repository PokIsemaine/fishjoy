//
// Created by zsl on 5/29/22.
//

#include "fishjoy/thread.hpp"

#include "fishjoy/log.hpp"
#include "fishjoy/mutex.hpp"
#include "fishjoy/util.hpp"

namespace fishjoy
{
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  static thread_local Thread* t_thread = nullptr;
  static thread_local std::string t_thread_name = "UNKNOWN";

  Thread* Thread::GetThis()
  {
    return t_thread;
  }

  const std::string& Thread::GetName()
  {
    return t_thread_name;
  }

  void Thread::SetName(const std::string& name)
  {
    if (t_thread != nullptr)
    {
      t_thread->m_name = name;
    }
    t_thread_name = name;
  }

  Thread::Thread(std::function<void()> callback, const std::string& name) : m_callback(callback), m_name(name)
  {
    if (name.empty())
    {
      m_name = "UNKNOWN";
    }
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (ret != 0)
    {
      FISHJOY_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << ret << " name=" << name;
      throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
  }

  Thread::~Thread()
  {
    if (m_thread != 0U)
    {
      pthread_detach(m_thread);
    }
  }

  void Thread::join()
  {
    if (m_thread)
    {
      auto ret = pthread_join(m_thread, nullptr);
      if (ret != 0)
      {
        FISHJOY_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << ret << " name=" << m_name;
        throw std::logic_error("pthread_join error");
      }
      m_thread = 0;
    }
  }

  void* Thread::run(void* arg)
  {
    auto* thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = fishjoy::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> callback;
    callback.swap(thread->m_callback);

    thread->m_semaphore.notify();

    callback();

    return nullptr;
  }

}  // namespace fishjoy
