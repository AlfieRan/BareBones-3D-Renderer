#include "types.h"
#include "utils.h"
#include "perspective.h"

#define MINIMUM_DISTANCE 0.5f

// Used to gewt the camera's rotation matrix
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


v3 cameraDisplayPos(Camera camera) {
	return (v3) {
        camera.position.x + (camera.displayOffset * camera.rotation.horizontal.sin),
        camera.position.y + (camera.displayOffset * camera.rotation.horizontal.cos),
		camera.position.z
    };
}

// Used to get a position relative to the camera
v2 posToCamera(v3 position, Camera camera) {
	vf3 tmp = matrixMult_vf3(camera.rotation.matrix, (v3) { position.x - camera.position.x, position.y - camera.position.y, position.z - camera.position.z });
	f32 mult = camera.displayPos.y / max(tmp.y, MINIMUM_DISTANCE);
	printf("\nMultipler (%lf) DisplayPos (%d, %d, %d) ", mult, camera.displayPos.x, camera.displayPos.y, camera.displayPos.z);
	return (v2) {
		(tmp.x * mult) + camera.displayPos.x,
		(tmp.y * mult) + camera.displayPos.y
	};
}