//
// Created by zsl on 7/23/22.
//

#ifndef FISHJOY_LIKELY_HPP
#define FISHJOY_LIKELY_HPP

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

#endif  // FISHJOY_LIKELY_HPP
