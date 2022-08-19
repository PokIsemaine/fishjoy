//
// Created by zsl on 7/23/22.
//

#ifndef FISHJOY_ASSERT_HPP
#define FISHJOY_ASSERT_HPP

#include <assert.h>
#include "fishjoy/utils/macro.hpp"

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

#endif  // FISHJOY_ASSERT_HPP
