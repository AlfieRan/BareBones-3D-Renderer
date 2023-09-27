#include "types.h"
#include "utils.h"
#include "perspective.h"

#define DISPLAY_DISTANCE 1
#define FOV_HORIZONTAL HALF_PI
#define FOV_VERTICAL HALF_PI

#define TWO_FOV_HORIZONTAL (FOV_HORIZONTAL * 2)
#define TWO_FOV_VERTICAL (FOV_VERTICAL * 2)

v2 point_to_screen(vf3 camera_rotation, vf3 camera_position, f64 screen_distance, vf3 point) {
	f64 cos_x = cos(camera_rotation.x);
	f64 sin_x = sin(camera_rotation.x);
	f64 cos_y = cos(camera_rotation.y);
	f64 sin_y = sin(camera_rotation.y);
	f64 cos_z = cos(camera_rotation.z);
	f64 sin_z = sin(camera_rotation.z);

	f64 x = point.x - camera_position.x;
	f64 y = point.y - camera_position.y;
	f64 z = point.z - camera_position.z;

	f64 sin_z_y_plus_cos_z_x = (sin_z * y) + (cos_z * x);
	f64 cos_z_y_minus_sin_z_x = (cos_z * y) - (sin_z * x);
	f64 d_z_y_sub = (cos_y * z) + (sin_y * sin_z_y_plus_cos_z_x);

	f64 d_x = (cos_y * sin_z_y_plus_cos_z_x) - (sin_y * z);
	f64 d_y = (sin_x * d_z_y_sub) + (cos_x * cos_z_y_minus_sin_z_x);
	f64 d_z = (cos_x * d_z_y_sub) - (sin_x * cos_z_y_minus_sin_z_x);

	f64 screen_prop_x = (screen_distance * d_x) / d_z;
	f64 screen_prop_y = (screen_distance  * d_y) / d_z;

	f64 screen_x = (screen_prop_x + 1) * HALF_SCREEN_WIDTH;
	f64 screen_y = (screen_prop_y + 1) * HALF_SCREEN_HEIGHT;

	printf("\nx: %f, y: %f", screen_x, screen_y);

	return (v2) { screen_x, screen_y };	
}