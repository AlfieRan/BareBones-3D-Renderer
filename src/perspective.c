#include "types.h"
#include "utils.h"
#include "perspective.h"

#define DISPLAY_DISTANCE 1
#define FOV_HORIZONTAL HALF_PI
#define FOV_VERTICAL HALF_PI

#define TWO_FOV_HORIZONTAL (FOV_HORIZONTAL * 2)
#define TWO_FOV_VERTICAL (FOV_VERTICAL * 2)

v2 point_to_screen(Camera camera,vf3 point) {
	f64 x = point.x - camera.position.x;
	f64 y = point.y - camera.position.y;
	f64 z = point.z - camera.position.z;


	f64 sin_z_y_plus_cos_z_x = (camera.rotation.z.sin * y) + (camera.rotation.z.cos * x);
	f64 cos_z_y_minus_sin_z_x = (camera.rotation.z.cos * y) - (camera.rotation.z.sin * x);
	f64 d_z_y_sub = (camera.rotation.y.cos * z) + (camera.rotation.y.sin * sin_z_y_plus_cos_z_x);

	f64 d_x = (camera.rotation.y.cos * sin_z_y_plus_cos_z_x) - (camera.rotation.y.sin * z);
	f64 d_y = (camera.rotation.x.sin * d_z_y_sub) + (camera.rotation.x.cos * cos_z_y_minus_sin_z_x);
	f64 d_z = (camera.rotation.x.cos * d_z_y_sub) - (camera.rotation.x.sin * cos_z_y_minus_sin_z_x);
	
	f64 screen_prop_x = (camera.screen_dist * d_x) / d_z;
	f64 screen_prop_y = (camera.screen_dist  * d_y) / d_z;

	f64 screen_x = (screen_prop_x + 1) * HALF_SCREEN_WIDTH;
	f64 screen_y = (screen_prop_y + 1) * HALF_SCREEN_HEIGHT;

	printf("\nx: %f, y: %f", screen_x, screen_y);

	return (v2) { screen_x, screen_y };	
}