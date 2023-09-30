#include "triangulation.h"

void triangles_from_cube(vf3 center, u32 radius, Material material, Triangle* triangles, usize* trianglesPointer, u16* numTriangles) {
	vf3 a = (vf3) { center.x - radius, center.y - radius, center.z - radius };
	vf3 b = (vf3) { center.x - radius, center.y - radius, center.z + radius };
	vf3 c = (vf3) { center.x - radius, center.y + radius, center.z - radius };
	vf3 d = (vf3) { center.x - radius, center.y + radius, center.z + radius };
	vf3 e = (vf3) { center.x + radius, center.y - radius, center.z - radius };
	vf3 f = (vf3) { center.x + radius, center.y - radius, center.z + radius };
	vf3 g = (vf3) { center.x + radius, center.y + radius, center.z - radius };
	vf3 h = (vf3) { center.x + radius, center.y + radius, center.z + radius };

	if (*trianglesPointer + 12 >= *numTriangles) {
		*numTriangles += TRIANGLE_BUFFER_SIZE;
		triangles = realloc(triangles, sizeof(Triangle) * *numTriangles);
	}

	triangles[*trianglesPointer] = (Triangle) { a, b, c, material };
	triangles[*trianglesPointer + 1] = (Triangle) { b, c, d, material };
	triangles[*trianglesPointer + 2] = (Triangle) { e, f, g, material };
	triangles[*trianglesPointer + 3] = (Triangle) { f, g, h, material };
	triangles[*trianglesPointer + 4] = (Triangle) { a, b, e, material };
	triangles[*trianglesPointer + 5] = (Triangle) { b, e, f, material };
	triangles[*trianglesPointer + 6] = (Triangle) { c, d, g, material };
	triangles[*trianglesPointer + 7] = (Triangle) { d, g, h, material };
	triangles[*trianglesPointer + 8] = (Triangle) { a, c, e, material };
	triangles[*trianglesPointer + 9] = (Triangle) { c, e, g, material };
	triangles[*trianglesPointer + 10] = (Triangle) { b, d, f, material };
	triangles[*trianglesPointer + 11] = (Triangle) { d, f, h, material };

	*trianglesPointer += 12;
};