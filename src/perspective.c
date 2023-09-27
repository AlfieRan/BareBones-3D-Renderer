#include "types.h"
#include "utils.h"
#include "perspective.h"

#define MINIMUM_DISTANCE 0.5f
#define FOV_HORIZONTAL TWO_PI
#define FOV_VERTICAL TWO_PI

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
	return rotation < FOV_HORIZONTAL || rotation > PI - FOV_HORIZONTAL;
}

bool verticalFOVCheck(f64 rotation) {
	return rotation < FOV_VERTICAL || rotation > PI - FOV_VERTICAL;
}

CameraPosResult posToCamera(vf3 position, Camera camera) {
	vf3 CD = (vf3) {
		(f32)(camera.rotation.horizontal.cos * camera.rotation.vertical.cos),
		(f32)(camera.rotation.horizontal.sin * camera.rotation.vertical.cos),
		camera.rotation.vertical.sin
	};

	vf3 CP = (vf3) {
		(f64)(position.x - camera.position.x),
		(f64)(position.y - camera.position.y),
		(f64)(position.z - camera.position.z)
	};

	f64 dot_horizontal = dot_vf2((vf2) { CD.x, CD.y }, (vf2) { CP.x, CP.y });
	f64 prod_horizontal_CP = (f64)(product_vf2((vf2) { CP.x, CP.y }));
	f64 prod_horizontal_CD = (f64)(product_vf2((vf2) { CD.x, CD.y }));
	f64 prod_horizontal = (f64)(prod_horizontal_CD * prod_horizontal_CP);
	f64 cos_theta_horizontal = (f64)(dot_horizontal / prod_horizontal);
	f64 theta_horizational = acos(cos_theta_horizontal);

	f64 cos_theta_vertical = (f64)((dot_vf2(
		(vf2) { CD.y, CD.z },
		(vf2) { CP.y, CP.z }
	)) / (
		product_vf2((vf2) { CD.y, CD.z }) * product_vf2((vf2) { CP.y, CP.z })
	));
	
	f64 theta_vertical = acos(cos_theta_vertical);

	// printf("\n[horizontal] theta: %lf, cos: %lf, product: %lf, dot: %lf, prod_CD: %lf, CD (%lf, %lf), Camera (r %lf, c %lf s %lf)", 
	// 	theta_horizational, cos_theta_horizontal, prod_horizontal, dot_horizontal, prod_horizontal_CD, CD.x, CD.y, camera.rotation.horizontal.raw, camera.rotation.horizontal.cos, camera.rotation.horizontal.sin);

	printf("\n[CAMERA] Position: (%i, %i, %i), Rotation: H (r %lf, c %lf, s %lf) V (r %lf, c %lf, s %lf)", 
		camera.position.x, camera.position.y, camera.position.z, camera.rotation.horizontal.raw, camera.rotation.horizontal.cos, 
		camera.rotation.horizontal.sin, camera.rotation.vertical.raw, camera.rotation.vertical.cos, camera.rotation.vertical.sin);

	if (isnan(theta_horizational) || isnan(theta_vertical)) { return (CameraPosResult) { false, (v2) { 0, 0 } }; }
	if (!horizontalFOVCheck(theta_horizational) || !verticalFOVCheck(theta_vertical)) { 
		// printf("\n[POS TO CAMERA] OUT OF SCREEN: Rot [%lf, %lf], CP (%lf, %lf, %lf), CD (%lf, %lf, %lf)", theta_horizational, theta_vertical, CP.x, CP.y, CP.z, CD.x, CD.y, CD.z);
		return (CameraPosResult) { false, (v2) { 0, 0 } }; 
	}

	f64 proportion_horizontal = theta_horizational / FOV_HORIZONTAL;
	f64 proportion_vertical = theta_vertical / FOV_VERTICAL;

	f64 x = proportion_horizontal * camera.screen.horizontal;
	f64 y = proportion_vertical * camera.screen.vertical;
	// printf("\n[SCREEN DATA] pos (%lf, %lf), props (%lf, %lf)", x, y, proportion_horizontal, proportion_vertical);

	// printf("\n[POS TO CAMERA] IN SCREEN: Rot [%lf, %lf], CP (%lf, %lf, %lf), CD (%lf, %lf, %lf)", theta_horizational, theta_vertical, CP.x, CP.y, CP.z, CD.x, CD.y, CD.z);
	return (CameraPosResult) { true, (v2) { x, y } };
}