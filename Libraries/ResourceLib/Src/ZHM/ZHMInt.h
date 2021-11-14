#pragma once

#include <cstdint>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef float float32;
typedef double float64;

#if ZHM_TARGET == 2012
typedef uint32_t zhmptr_t;
typedef int32_t zhmptrdiff_t;
#else
typedef uint64_t zhmptr_t;
typedef int64_t zhmptrdiff_t;
#endif
