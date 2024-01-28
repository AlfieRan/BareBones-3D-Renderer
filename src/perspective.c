#include "types.h"
#include "utils.h"
#include "perspective.h"

bool is_in_front_of_camera(Camera camera, vf3 point) {
	vf3 camera_to_point = (vf3) {
		point.x - camera.position.x,
		point.y - camera.position.y,
		point.z - camera.position.z
	};

	f64 dot = camera.rotation.z.raw * product_vf3(camera_to_point);

	return dot > 0;
}

ScreenPoint point_to_screen(Camera camera, vf3 point) {
	f64 x = point.x - camera.position.x;
	f64 y = point.y - camera.position.y;
	f64 z = point.z - camera.position.z;

	f64 sin_z_y_plus_cos_z_x = (camera.rotation.z.sin * y) + (camera.rotation.z.cos * x);
	f64 cos_z_y_minus_sin_z_x = (camera.rotation.z.cos * y) - (camera.rotation.z.sin * x);
	f64 d_z_y_sub = (camera.rotation.y.cos * z) + (camera.rotation.y.sin * sin_z_y_plus_cos_z_x);

	f64 d_x = (camera.rotation.y.cos * sin_z_y_plus_cos_z_x) - (camera.rotation.y.sin * z);
	f64 d_y = (camera.rotation.x.sin * d_z_y_sub) + (camera.rotation.x.cos * cos_z_y_minus_sin_z_x);
	f64 d_z = (camera.rotation.x.cos * d_z_y_sub) - (camera.rotation.x.sin * cos_z_y_minus_sin_z_x);
	
	if (d_z == 0) {
		printf("d_z is 0\n");
		return (ScreenPoint) { (v2) { 0, 0 }, false, 0 };
	}

	f64 inv_d_z = 1 / d_z;
	f64 screen_prop_x = (camera.screen_dist * d_x) * inv_d_z;
	f64 screen_prop_y = (camera.screen_dist * d_y) * inv_d_z;

	f64 screen_x = (screen_prop_x + 1) * HALF_SCREEN_WIDTH;
	f64 screen_y = ((screen_prop_y * ASPECT_RATIO) + 1) * HALF_SCREEN_HEIGHT;

	f64 sqr_depth = x * x + y * y + z * z;
	bool in_front = d_z > 0;
	// bool in_front = true;

	return (ScreenPoint){(v2) { screen_x, screen_y }, in_front, sqr_depth};
}

ClampPosition clamp_position(int a, int b, int minimum, int maximum) {
	int low = min(a, b);
	int high = max(a, b);
	low = clamp(low, minimum, maximum);
	high = clamp(high, minimum, maximum);

	return (ClampPosition) { low, high };
}

vf3 cam_pos_minus_screen(Camera camera) {
    // Calculate the forward vector from the camera's rotation angles
    // Forward vector components
    f64 Fx = camera.rotation.y.cos * camera.rotation.x.cos;
    f64 Fy = camera.rotation.x.sin;
    f64 Fz = camera.rotation.y.sin * camera.rotation.x.cos;

    // Calculate the camera origin by moving back along the forward vector
    vf3 origin;
    origin.x = camera.position.x - Fx * camera.screen_dist;
    origin.y = camera.position.y - Fy * camera.screen_dist;
    origin.z = camera.position.z - Fz * camera.screen_dist;

    return origin;
}