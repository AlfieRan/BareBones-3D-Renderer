#include "types.h"

#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__

v2 point_to_screen(vf3 camera_rotation, vf3 camera_position, f64 screen_distance, vf3 point);

#endif /* PERSPECTIVE_H__*/