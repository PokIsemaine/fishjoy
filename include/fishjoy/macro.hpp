//
// Created by zsl on 6/9/22.
//

#ifndef FISHJOY_MACRO_HPP
#define FISHJOY_MACRO_HPP

#include <string>
#include <cassert>
#include "util.hpp"

/// 协程调度 N：M 可能降低 CPU CACHE MISS
#if defined __GNUC__ || defined __llvm__
/// LIKELY 宏的封装, 告诉编译器优化,条件大概率成立
#define FISHJOY_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKELY 宏的封装, 告诉编译器优化,条件大概率不成立
#define FISHJOY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define FISHJOY_LIKELY(x) (x)
#define FISHJOY_UNLIKELY(x) (x)
#endif


/// 断言宏封装
#define FISHJOY_ASSERT(x) \
    if(!(x)) { \
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << fishjoy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

/// 断言宏封装
#define FISHJOY_ASSERT2(x, w) \
    if(!(x)) { \
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << fishjoy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
#endif  // FISHJOY_MACRO_HPP
