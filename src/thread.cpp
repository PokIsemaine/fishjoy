//
// Created by zsl on 5/29/22.
//

#include "fishjoy/thread.hpp"

#include "fishjoy/log.hpp"
#include "fishjoy/util.hpp"

namespace fishjoy
{
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  Semaphore::Semaphore(uint32_t count)
  {
    if(sem_init(&m_semaphore, 0, count) != 0) {
      throw std::logic_error("sem_init error");
    }
  }

  Semaphore::~Semaphore()
  {
    sem_destroy(&m_semaphore);
  }

  void Semaphore::wait() {
    if(sem_wait(&m_semaphore) != 0) {
      throw std::logic_error("sem_wait error");
    }
  }

  void Semaphore::notify()
  {
    if(sem_post(&m_semaphore) != 0) {
      throw std::logic_error("sem_post error");
    }
  }

  static thread_local Thread* t_thread = nullptr;
  static thread_local std::string t_thread_name = "UNKOWN";


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
      m_name = "UNKNOW";
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
    if (m_thread)
    {
      pthread_detach(m_thread);
    }
  }

  void Thread::join()
  {
    if (m_thread)
    {
      int ret = pthread_join(m_thread, nullptr);
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

    std::function<void()> cb;
    cb.swap(thread->m_callback);

    thread->m_semaphore.notify();

    cb();

    return nullptr;
  }


}  // namespace fishjoy
