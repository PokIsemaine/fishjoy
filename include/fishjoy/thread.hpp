//
// Created by zsl on 5/29/22.
//

#ifndef FISHJOY_THREAD_HPP
#define FISHJOY_THREAD_HPP

#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>
#include <string>

namespace fishjoy {
  class Thread {
   public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id;}
    const std::string& getName() const { return m_name;}

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
   private:
    //互斥量、互斥信号不能拷贝构造
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    static void* run(void* arg);
   private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
  };
}


#endif  // FISHJOY_THREAD_HPP
