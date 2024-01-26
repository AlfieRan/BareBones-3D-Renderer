#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__
#include "types.h"

ScreenPoint point_to_screen(Camera camera, vf3 point);
ClampPosition clamp_position(int a, int b, int minimum, int maximum);
vf3 cam_pos_minus_screen(Camera camera);

#endif /* PERSPECTIVE_H__*/