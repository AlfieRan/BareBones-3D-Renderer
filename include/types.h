#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <SDL3/SDL.h>

#ifndef TYPES_H__
#define TYPES_H__

// CONSTANTS AND MACROS =======================================================

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CROSSHAIR_SIZE 5
#define ROTATION_SPEED 0.001f
#define MOVEMENT_SPEED 5
#define VERTICAL_FOV 1

#define HALF_SCREEN_WIDTH (SCREEN_WIDTH / 2)
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT / 2)
#define ASPECT_RATIO ((f64) SCREEN_WIDTH / (f64) SCREEN_HEIGHT)

#define GREEN 0xFF00FFAA
#define PURPLE 0xFFFF00AA
#define RED 0xFF0000AA
#define BLUE 0xFF0000FF

#define PI 3.14159265358979323846f
#define HALF_PI 1.57079632679489661923f
#define TWO_PI 6.28318530717958647692f

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
typedef struct { f64 x,y; } vf2;
typedef struct { f64 x,y,z; } vf3;
typedef struct { f32 a,b,c,d,e,f,g,h,i; } m3;
typedef struct { f32 a,b,c,d; } m4_row;
typedef struct { m4_row a,b,c,d; } m4;
typedef struct { vf3 pos, dir; } Ray;
typedef struct { f64 hor, ver; } Angle3D;

#define vf3_to_v3(v) (v3) { (i32) v.x, (i32) v.y, (i32) v.z }

// Complex Base types
typedef struct { int change_x, change_y; } MouseMovement;
typedef struct { f32 raw, sin, cos; } Angle;
typedef struct { Angle x, y, z; } CameraRotation;
typedef struct { f32 horizontal, vertical; } FOV;
typedef struct { u16 horizontal, vertical; } SCREEN;
typedef struct { vf3 position; CameraRotation rotation; f64 screen_dist; } Camera;

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture, *debug;
	u32 *pixels;
	bool quit;

	MouseMovement mouse;
	Camera camera;
} State;

// Shapes
typedef struct { v3 center; f32 length; u32 color; } Cube;
typedef struct { v3 center; f32 width; f32 height; Angle rotation; u32 color; } Plane;


// Return types
typedef struct { bool success; v2 position; } CameraPosResult;
typedef struct { bool success; v2 pointA, pointB; } ScreenLineResult;
typedef struct { bool success; Angle3D angle; } CameraAngleResult;

#endif /* TYPES_H__*/