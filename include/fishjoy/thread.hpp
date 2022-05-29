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

#include "noncopyable.hpp"

namespace fishjoy
{
  class Semaphore : Noncopyable {
   public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

   private:
    sem_t m_semaphore;
  };

  template<typename T>
  struct ScopedLockImpl {
   public:
      ScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
        mutex.lock();
        m_locked = true;
      }

      ~ScopedLockImpl() {
        unlock();
      }

      void lock(){
        if(!m_locked) {
          m_mutex.lock();
          m_locked = true;
        }
      }

      void unlock() {
        if(m_locked) {
          m_mutex.unlock();
          m_locked = false;
        }
      }
   private:
    T& m_mutex;
    bool m_locked;
  };

  template<typename T>
  struct ReadScopedLockImpl {
   public:
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
      mutex.lock();
      m_locked = true;
    }

    ~ReadScopedLockImpl() {
      unlock();
    }

    void lock(){
      if(!m_locked) {
        m_mutex.rdlock();
        m_locked = true;
      }
    }

    void unlock() {
      if(m_locked) {
        m_mutex.unlock();
        m_locked = false;
      }
    }
   private:
    T& m_mutex;
    bool m_locked;
  };

  template<typename T>
  struct WriteScopedLockImpl {
   public:
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
      mutex.lock();
      m_locked = true;
    }

    ~WriteScopedLockImpl() {
      unlock();
    }

    void lock(){
      if(!m_locked) {
        m_mutex.lock();
        m_locked = true;
      }
    }

    void unlock() {
      if(m_locked) {
        m_mutex.unlock();
        m_locked = false;
      }
    }
   private:
    T& m_mutex;
    bool m_locked;
  };

  class RWMutex : Noncopyable {
   public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;

    RWMutex() {
      pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
      pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
      pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
      pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
      pthread_rwlock_unlock(&m_lock);
    }
   private:
    pthread_rwlock_t m_lock;
  };
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
