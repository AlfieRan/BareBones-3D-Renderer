#include <stdlib.h>
#include <stdio.h>

#ifndef TYPES_H__
#define TYPES_H__

// TYPE DEFINITIONS ===========================================================
typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
typedef ssize_t  isize;
typedef struct { i32 x,y,z; } Pos;
typedef struct { Pos center; u8 length; u32 color; } Cube;

#endif /* TYPES_H__*/