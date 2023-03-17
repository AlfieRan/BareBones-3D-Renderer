#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <SDL3/SDL.h>

#ifndef TYPES_H__
#define TYPES_H__

// Simple types
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

// Vectors and Matrices
typedef struct { i32 x,y,z; } v3;
typedef struct { i32 x,y; } v2;
typedef struct { f32 x,y,z; } vf3;
typedef struct { f32 a,b,c,d,e,f,g,h,i; } m3;

#define vf3_to_v3(v) (v3) { (i32) v.x, (i32) v.y, (i32) v.z }

// Complex Base types
typedef struct { f32 raw, sin, cos; } Angle;
typedef struct { Angle horizontal, vertical; m3 matrix; } Rotation;
typedef struct { v3 position; v3 display; Rotation rotation; } Camera;

// Shapes
typedef struct { v3 center; f32 length; u32 color; } Cube;


#endif /* TYPES_H__*/