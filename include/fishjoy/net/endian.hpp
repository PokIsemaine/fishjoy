//
// Created by zsl on 7/8/22.
//

#ifndef FISHJOY_ENDIAN_HPP
#define FISHJOY_ENDIAN_HPP

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

#define FISHJOY_LITTLE_ENDIAN 1
#define FISHJOY_BIG_ENDIAN 2

// TODO std::endian https://en.cppreference.com/w/cpp/types/endian
// TODO std::byte https://en.cppreference.com/w/cpp/types/byte
// TODO std::byteswap https://en.cppreference.com/w/cpp/numeric/byteswap 手动实现
namespace fishjoy {

  /**
 * @brief 8字节类型的字节序转化
   */
  template <class T>
  typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
  byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
  }

  /**
 * @brief 4字节类型的字节序转化
   */
  template <class T>
  typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
  byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
  }

  /**
 * @brief 2字节类型的字节序转化
   */
  template <class T>
  typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
  byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
  }

  // endian.h
#if BYTE_ORDER == BIG_ENDIAN
#define FISHJOY_BYTE_ORDER FISHJOY_BIG_ENDIAN
#else
#define FISHJOY_BYTE_ORDER FISHJOY_LITTLE_ENDIAN
#endif

#if FISHJOY_BYTE_ORDER == FISHJOY_BIG_ENDIAN

  /**
    * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
   */
  template <class T>
  T byteswapOnLittleEndian(T t) {
    return t;
  }

  /**
    * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
   */
  template <class T>
  T byteswapOnBigEndian(T t) {
    return byteswap(t);
  }
#else

  /**
    * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
   */
  template <class T>
  T byteswapOnLittleEndian(T t) {
    return byteswap(t);
  }

  /**
    * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
   */
  template <class T>
  T byteswapOnBigEndian(T t) {
    return t;
  }
#endif

}

#endif  // FISHJOY_ENDIAN_HPP
