//
// Created by zsl on 5/30/22.
//

#ifndef FISHJOY_NONCOPYABLE_HPP
#define FISHJOY_NONCOPYABLE_HPP

namespace fishjoy
{
  class Noncopyable
  {
   public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
  };
}  // namespace fishjoy

#endif  // FISHJOY_NOCOPYABLE_HPP
