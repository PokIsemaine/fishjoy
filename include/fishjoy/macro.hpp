//
// Created by zsl on 6/9/22.
//

#ifndef FISHJOY_MACRO_HPP
#define FISHJOY_MACRO_HPP

#include <string>
#include <cassert>
#include "util.hpp"

#define FISHJOY_ASSERT(x) \
    if(!(x)) { \
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << fishjoy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define FISHJOY_ASSERT2(x, w) \
    if(!(x)) { \
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << fishjoy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
#endif  // FISHJOY_MACRO_HPP
