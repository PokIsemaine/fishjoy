//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_SINGLETON_H
#define FISHJOY_SINGLETON_H

#include <memory>

/**
 * C++11 基于静态局部对象实现
 * C++11标准保证：如果多个线程试图同时初始化同一静态局部对象，则初始化严格只发生一次
 * https://en.cppreference.com/w/cpp/language/storage_duration#Static_local_variables
 *
 */


/**
 * @brief 单例模式封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */


namespace fishjoy {

  namespace {

    template<class T, class X, int N>
    T& GetInstanceX() {
      static T v;
      return v;
    }

    template<class T, class X, int N>
    std::shared_ptr<T> GetInstancePtr() {
      static std::shared_ptr<T> v(new T);
      return v;
    }

  }



  template<typename T, typename X = void, int N = 0>
  /**
     * @brief 返回单例裸指针
   */
  class Singleton {
   public:
    static T* GetInstance() {
      static T v;
      return &v;
    }

   public:
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
   private:
    Singleton() = default;
    ~Singleton() = default;
  };


  /**
   * @brief 单例模式智能指针封装类
   * @details T 类型
   *          X 为了创造多个实例对应的Tag
   *          N 同一个Tag创造多个实例索引
   */
  template<typename T, typename X = void, int N = 0>
  class SingletonPtr {
   public:
    /**
     * @brief 返回单例智能指针
     */
    static std::shared_ptr<T> GetInstance() {
      static std::shared_ptr<T> v(new T);
      return v;
    }

   public:
    SingletonPtr(const SingletonPtr&) = delete;
    SingletonPtr(SingletonPtr&&) = delete;
    SingletonPtr& operator=(const SingletonPtr&) = delete;
    SingletonPtr& operator=(SingletonPtr&&) = delete;
   private:
    SingletonPtr() = default;
    ~SingletonPtr() = default;


  };
}  // namespace fishjoy

#endif  // FISHJOY_SINGLETON_H
