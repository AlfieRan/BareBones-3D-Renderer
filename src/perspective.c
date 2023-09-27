#include "types.h"
#include "utils.h"
#include "perspective.h"

#define DISPLAY_DISTANCE 1
#define FOV_HORIZONTAL HALF_PI
#define FOV_VERTICAL HALF_PI

#define TWO_FOV_HORIZONTAL (FOV_HORIZONTAL * 2)
#define TWO_FOV_VERTICAL (FOV_VERTICAL * 2)

// Used to get the camera's rotation matrix
m3 rotationMatrix(Angle horizontal, Angle vertical) {
	return matrixMult_m3(
		(m3) { 	
				horizontal.cos, 	-horizontal.sin, 	0.0f, 
				horizontal.sin, 	horizontal.cos, 	0.0f,  
				0.0f, 				0.0f, 				1.0f
		}, 
		(m3) {	
				1.0f, 				0.0f, 				0.0f, 
				0.0f, 				vertical.cos, 		-vertical.sin, 
				0.0f, 				vertical.sin, 		vertical.cos
		}
	);
}


bool horizontalFOVCheck(f64 rotation) {
	return rotation < FOV_HORIZONTAL && rotation > -FOV_HORIZONTAL;
}

bool verticalFOVCheck(f64 rotation) {
	return rotation < FOV_VERTICAL && rotation > -FOV_VERTICAL;
}

CameraAngleResult angleToCamera(vf3 position, Camera camera) {
	vf3 CD_Raw = (vf3) {
		(f64)(camera.rotation.horizontal.cos * camera.rotation.vertical.cos),
		(f64)(camera.rotation.horizontal.sin * camera.rotation.vertical.cos),
		camera.rotation.vertical.sin
	};

	f64 CD_Raw_Prod = product_vf3(CD_Raw);
	f64 CD_Mult = (f64)(DISPLAY_DISTANCE / CD_Raw_Prod);

	vf3 CD = (vf3) {
		(f64)(CD_Raw.x * CD_Mult),
		(f64)(CD_Raw.y * CD_Mult),
		(f64)(CD_Raw.z * CD_Mult)
	};

	printf("\nCD (%lf, %lf, %lf)", CD.x, CD.y, CD.z);

	// TODO: this position needs to be cast from infront of the camera rather than the origin to allow perspective to work
	vf3 CP = (vf3) {
		(f64)(position.x - camera.position.x),
		(f64)(position.y - camera.position.y),
		(f64)(position.z - camera.position.z)
	};

	f64 dot_horizontal = dot_vf2((vf2) { CD.x, CD.y }, (vf2) { CP.x, CP.y });
	f64 det_horizontal = det_vf2((vf2) { CD.x, CD.y }, (vf2) { CP.x, CP.y });
	f64 theta_horizational = atan2(det_horizontal, dot_horizontal);

	f64 dot_vertical = dot_vf2((vf2) { CD.y, CD.z }, (vf2) { CP.y, CP.z });
	f64 det_vertical = det_vf2((vf2) { CD.y, CD.z }, (vf2) { CP.y, CP.z });
	f64 theta_vertical = atan2(det_vertical, dot_vertical);

	if (isnan(theta_horizational) || isnan(theta_vertical)) { return (CameraAngleResult) { false, (Angle3D) { 0, 0 } }; }
	return (CameraAngleResult) { true, (Angle3D) { theta_horizational, theta_vertical } };
}

CameraPosResult posToCamera(vf3 position, Camera camera) {
	CameraAngleResult angle = angleToCamera(position, camera);

	if (!angle.success) { return (CameraPosResult) { false, (v2) { 0, 0 } }; }

	f64 theta_horizational = angle.angle.hor;
	f64 theta_vertical = angle.angle.ver;

	if (!horizontalFOVCheck(theta_horizational) || !verticalFOVCheck(theta_vertical)) { 
		return (CameraPosResult) { false, (v2) { 0, 0 } }; 
	}

	f64 proportion_horizontal = theta_horizational / TWO_FOV_HORIZONTAL;
	f64 proportion_vertical = theta_vertical / TWO_FOV_HORIZONTAL;

	f64 x = (proportion_horizontal + 0.5) * camera.screen.horizontal;
	f64 y = (proportion_vertical + 0.5) * camera.screen.vertical;

	if (x > SCREEN_WIDTH || x < 0 || y > SCREEN_HEIGHT || y < 0) {
		printf("\n[ERROR] Tried to draw outside of screen.\n\tPosition (%lf, %lf)\n\tProportion (%lf, %lf)\n\tRotation From Camera (%lf, %lf)", x, y, proportion_horizontal, proportion_vertical, theta_horizational, theta_vertical);
		return (CameraPosResult) { false, (v2) { 0, 0 } }; 
	}

	return (CameraPosResult) { true, (v2) { x, y } };
}

// ScreenLineResult lineToScreen(vf3 a, vf3 b, Camera camera) {

	
// 	return (ScreenLineResult) { false, (v2) { 0, 0 }, (v2) { 0, 0 } }; 
// }