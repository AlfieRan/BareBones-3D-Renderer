#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__
#include "types.h"

ScreenPoint point_to_screen(Camera camera, vf3 point);
ClampPosition clamp_position(int a, int b, int minimum, int maximum);

#endif /* PERSPECTIVE_H__*/