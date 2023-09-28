#include "types.h"

#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__

typedef struct { int low, high; } ClampPosition;
typedef struct { v2 pos; bool in_front; } ScreenPoint;

ScreenPoint point_to_screen(Camera camera, vf3 point);
ClampPosition clamp_position(int a, int b, int minimum, int maximum);

#endif /* PERSPECTIVE_H__*/