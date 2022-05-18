//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_SINGLETON_H
#define FISHJOY_SINGLETON_H


namespace fishjoy {
  template<typename T, typename X = void, int N = 0>
  class Singleton {
   public:
    static T* GetInstance() {
      static T v;
      return &v;
    }
  };

  template<typename T, typename X = void, int N = 0>
  class SingletonPtr {
   public:
    static std::shared_ptr<T> GetInstance() {
      static std::shared_ptr<T> v(new T);
      return v;
    }
  };
}

#endif  // FISHJOY_SINGLETON_H
