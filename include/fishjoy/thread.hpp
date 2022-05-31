//
// Created by zsl on 5/29/22.
//

#ifndef FISHJOY_THREAD_HPP
#define FISHJOY_THREAD_HPP

#include <pthread.h>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <semaphore.h>
#include <atomic>
#include "mutex.hpp"
#include "noncopyable.hpp"

namespace fishjoy
{
  class Thread : Noncopyable
  {
   public:
    using ptr = std::shared_ptr<Thread>;
    Thread(std::function<void()> callback, const std::string& name);
    ~Thread();

    pid_t getId() const
    {
      return m_id;
    }
    const std::string& getName() const
    {
      return m_name;
    }

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);

    //互斥量、互斥信号不能拷贝构造
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

   private:
    static void* run(void* arg);

    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_callback;
    std::string m_name;

    Semaphore m_semaphore;
  };
}  // namespace fishjoy

#endif  // FISHJOY_THREAD_HPP
