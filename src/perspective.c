#include "types.h"
#include "utils.h"
#include "perspective.h"

#define MINIMUM_DISTANCE 0.5f
#define FOV_HORIZONTAL 1
#define FOV_VERTICAL 1

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


CameraPosResult posToCamera(v3 position, Camera camera) {
	// printf("\n[POS TO CAMERA] Horizontal: (raw: %lf, cos: %lf, sin: %lf), Vertical: (raw: %lf, cos: %lf, sin: %lf)", camera.rotation.horizontal.raw, camera.rotation.horizontal.cos, camera.rotation.horizontal.sin, camera.rotation.vertical.raw, camera.rotation.vertical.cos, camera.rotation.vertical.sin);
	// printf("\n[POS TO CAMERA] Position: (%i, %i, %i), Camera: (%i, %i, %i)", position.x, position.y, position.z, camera.position.x, camera.position.y, camera.position.z);

	vf3 CD = (vf3) {
		(f32)(camera.rotation.horizontal.cos * camera.rotation.vertical.cos),
		(f32)(camera.rotation.horizontal.sin * camera.rotation.vertical.cos),
		camera.rotation.vertical.sin
	};

	vf3 CP = (vf3) {
		position.x - camera.position.x,
		position.y - camera.position.y,
		position.z - camera.position.z
	};

	double theta_horizational = acos((dot_vf2(
		(vf2) { CD.x, CD.y },
		(vf2) { CP.x, CP.y }
	)) / (
		product_vf2((vf2) { CD.x, CD.y }) * product_vf2((vf2) { CP.x, CP.y })
	));

	double theta_vertical = acos((dot_vf2(
		(vf2) { CD.y, CD.z },
		(vf2) { CP.y, CP.z }
	)) / (
		product_vf2((vf2) { CD.y, CD.z }) * product_vf2((vf2) { CP.y, CP.z })
	));

	if (isnan(theta_horizational) || isnan(theta_vertical)) {
		// printf("\n\n[POS TO CAMERA] NAN: Horizontal: %lf, Vertical: %lf", theta_horizational, theta_vertical);
		// printf("\n\n[POS TO CAMERA] NAN: Horizontal: %lf, Vertical: %lf", theta_horizational, theta_vertical);
		// printf("\nCD: (%lf, %lf, %lf), CP: (%lf, %lf, %lf)", CD.x, CD.y, CD.z, CP.x, CP.y, CP.z);
		return (CameraPosResult) { false, (v2) { 0, 0 } };
	}

	if (theta_horizational > FOV_HORIZONTAL || theta_vertical > FOV_VERTICAL) {
		// printf("\n\n[POS TO CAMERA] OUT OF SCREEN: Horizontal: %lf, Vertical: %lf", theta_horizational, theta_vertical);
		// printf("\nCD: (%lf, %lf, %lf), CP: (%lf, %lf, %lf)", CD.x, CD.y, CD.z, CP.x, CP.y, CP.z);
		return (CameraPosResult) { false, (v2) { 0, 0 } };
	}

	double proportion_horizontal = theta_horizational / FOV_HORIZONTAL;
	double proportion_vertical = theta_vertical / FOV_VERTICAL;

	double x = proportion_horizontal * camera.screen.horizontal;
	double y = proportion_vertical * camera.screen.vertical;

	printf("\n[POS TO CAMERA] IN SCREEN: Rot [%lf, %lf], CP (%lf, %lf, %lf)", theta_horizational, theta_vertical, CP.x, CP.y, CP.z);
	return (CameraPosResult) { true, (v2) { x, y } };
}