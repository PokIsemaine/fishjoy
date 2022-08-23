//
// Created by zsl on 8/21/22.
//
#include <semaphore.h>
#include "fishjoy/utils/noncopyable.hpp"
#include <atomic>

#ifndef FISHJOY_SEMAPHORE_HPP
#define FISHJOY_SEMAPHORE_HPP
namespace fishjoy {
  class Semaphore : Noncopyable {
   public:
    explicit Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

   private:
    sem_t m_semaphore;
  };
}
#endif  // FISHJOY_SEMAPHORE_HPP
