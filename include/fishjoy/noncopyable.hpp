//
// Created by zsl on 5/30/22.
//

#ifndef FISHJOY_NOCOPYABLE_HPP
#define FISHJOY_NOCOPYABLE_HPP

namespace fishjoy {
  class Noncopyable {
   public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
  };
}

#endif  // FISHJOY_NOCOPYABLE_HPP
