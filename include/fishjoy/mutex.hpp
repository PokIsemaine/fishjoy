//
// Created by zsl on 5/31/22.
//

#ifndef FISHJOY_MUTEX_HPP
#define FISHJOY_MUTEX_HPP

#include <semaphore.h>
#include <atomic>
#include "noncopyable.hpp"

namespace fishjoy {
  class Semaphore : Noncopyable {
   public:

    void wait();
    void notify();

   public:  // 构造函数，析构函数

    Semaphore(uint32_t count = 0);
    ~Semaphore();

   private:
    sem_t m_semaphore;
  };

  template<typename T>
  struct ScopedLockImpl {
   public:

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

   public:  // 构造函数，析构函数

    ScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
      mutex.lock();
      m_locked = true;
    }

    ~ScopedLockImpl() {
      unlock();
    }

   private:

    T& m_mutex;
    bool m_locked;
  };

  template<typename T>
  struct ReadScopedLockImpl {
   public:

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

   public:  //  构造函数，析构函数

    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
      mutex.rdlock();
      m_locked = true;
    }

    ~ReadScopedLockImpl() {
      unlock();
    }

   private:
    T& m_mutex;
    bool m_locked;
  };

  template<typename T>
  struct WriteScopedLockImpl {
   public:

    void lock(){
      if(!m_locked) {
        m_mutex.wrlock();
        m_locked = true;
      }
    }

    void unlock() {
      if(m_locked) {
        m_mutex.unlock();
        m_locked = false;
      }
    }

    public: // 构造函数，析构函数

      WriteScopedLockImpl(T& mutex)
          :m_mutex(mutex) {
        mutex.wrlock();
        m_locked = true;
      }

      ~WriteScopedLockImpl() {
        unlock();
      }

   private:

    T& m_mutex;
    bool m_locked;
  };


  class RWMutex :Noncopyable {
   public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;

    void rdlock() {
      pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
      pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
      pthread_rwlock_unlock(&m_lock);
    }


   public:  // 构造函数，析构函数

    RWMutex() {
      pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
      pthread_rwlock_destroy(&m_lock);
    }

   private:
    pthread_rwlock_t m_lock;
  };

  class Mutex :Noncopyable
  {
   public:
    using Lock = ScopedLockImpl<Mutex>;


    void lock() {
      pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
      pthread_mutex_unlock(&m_mutex);
    }

   public:  // 构造函数，析构函数

    Mutex() {
      pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex() {
      pthread_mutex_destroy(&m_mutex);
    }

   private:

    pthread_mutex_t m_mutex;
  };

  class NullMutex : Noncopyable{
    using Lock = ScopedLockImpl<NullMutex>;

   public:

    void lock() {}
    void unlock() {}

   public:  // 构造函数，析构函数

    NullMutex() {}
    ~NullMutex() {}
  };

  class Spinlock : Noncopyable{
   public:

    using Lock = ScopedLockImpl<Spinlock>;


    void lock() {
      pthread_spin_lock(&m_mutex);
    }
    void unlock() {
      pthread_spin_unlock(&m_mutex);
    }

   public:  // 构造函数，析构函数

    Spinlock() {
      pthread_spin_init(&m_mutex,0);
    }

    ~Spinlock() {
      pthread_spin_destroy(&m_mutex);
    }

   private:

    pthread_spinlock_t m_mutex;
  };

  class CASLock :Noncopyable{
   public:

    using Lock = ScopedLockImpl<CASLock>;

    void lock() {
      while (std::atomic_flag_test_and_set_explicit(&m_mutex,std::memory_order_acquire));
    }

    void unlock() {
      std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }

   public:  // 构造函数，析构函数

    CASLock() {
      m_mutex.clear();
    }
    ~CASLock() {

    }

   private:

    volatile std::atomic_flag m_mutex;
  };
}
#endif  // FISHJOY_MUTEX_HPP
