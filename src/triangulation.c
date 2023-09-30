#include "triangulation.h"

void triangles_from_cube(State state, vf3 center, u32 radius, Material material, Triangle* triangles, usize* trianglesPointer, u16* numTriangles) {
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

	triangles[*trianglesPointer] = (Triangle) { a, b, c, material, getTriangleDist(a, b, c, state.camera.position), false };
	triangles[*trianglesPointer + 1] = (Triangle) { b, c, d, material, getTriangleDist(b, c, d, state.camera.position), false };
	triangles[*trianglesPointer + 2] = (Triangle) { e, f, g, material, getTriangleDist(e, f, g, state.camera.position), false };
	triangles[*trianglesPointer + 3] = (Triangle) { f, g, h, material, getTriangleDist(f, g, h, state.camera.position), false };
	triangles[*trianglesPointer + 4] = (Triangle) { a, b, e, material, getTriangleDist(a, b, e, state.camera.position), false };
	triangles[*trianglesPointer + 5] = (Triangle) { b, e, f, material, getTriangleDist(b, e, f, state.camera.position), false };
	triangles[*trianglesPointer + 6] = (Triangle) { c, d, g, material, getTriangleDist(c, d, g, state.camera.position), false };
	triangles[*trianglesPointer + 7] = (Triangle) { d, g, h, material, getTriangleDist(d, g, h, state.camera.position), false };
	triangles[*trianglesPointer + 8] = (Triangle) { a, c, e, material, getTriangleDist(a, c, e, state.camera.position), false };
	triangles[*trianglesPointer + 9] = (Triangle) { c, e, g, material, getTriangleDist(c, e, g, state.camera.position), false };
	triangles[*trianglesPointer + 10] = (Triangle) { b, d, f, material, getTriangleDist(b, d, f, state.camera.position), false };
	triangles[*trianglesPointer + 11] = (Triangle) { d, f, h, material, getTriangleDist(d, f, h, state.camera.position), false };

	*trianglesPointer += 12;
};

f64 getTriangleDist(vf3 a, vf3 b, vf3 c, vf3 camera) {
	vf3 center = (vf3) { (a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3 };
	f64 dist = dist_vf3(center, camera);
	return dist < 0 ? -dist : dist;
}

v3 getChunk(vf3 pos) {
	return (v3) { (i32) (pos.x / CHUNK_SIZE), (i32) (pos.y / CHUNK_SIZE), (i32) (pos.z / CHUNK_SIZE) };
}