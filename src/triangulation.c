#include "triangulation.h"

UpdateTriangleData triangles_from_cube(State state, vf3 center, u32 radius, Material material) {
	// printf("\n[TRIANGLES] Getting 8 points for cube");
	vf3 a = (vf3) { center.x - radius, center.y - radius, center.z - radius };
	vf3 b = (vf3) { center.x - radius, center.y - radius, center.z + radius };
	vf3 c = (vf3) { center.x - radius, center.y + radius, center.z - radius };
	vf3 d = (vf3) { center.x - radius, center.y + radius, center.z + radius };
	vf3 e = (vf3) { center.x + radius, center.y - radius, center.z - radius };
	vf3 f = (vf3) { center.x + radius, center.y - radius, center.z + radius };
	vf3 g = (vf3) { center.x + radius, center.y + radius, center.z - radius };
	vf3 h = (vf3) { center.x + radius, center.y + radius, center.z + radius };

	// printf("\n[TRIANGLES] Checking if we need to realloc triangles buffer (%ld / %ld)", state.trianglesPointer + 12, state.numTriangles);
	usize buffer_increase = 0;
	if (state.trianglesPointer + 12 >= state.numTriangles) {
		// printf("\n[TRIANGLES] Reallocating triangles buffer");
		buffer_increase = TRIANGLE_BUFFER_SIZE;
		state.numTriangles += buffer_increase;
		state.triangles = realloc(state.triangles, sizeof(Triangle) * state.numTriangles);
	}

	// printf("\n[TRIANGLES] Adding 12 triangles to the buffer");
	state.triangles[state.trianglesPointer] = (Triangle) { a, b, c, material, getTriangleDist(a, b, c, state.camera.position) };
	state.triangles[state.trianglesPointer + 1] = (Triangle) { b, c, d, material, getTriangleDist(b, c, d, state.camera.position) };
	state.triangles[state.trianglesPointer + 2] = (Triangle) { e, f, g, material, getTriangleDist(e, f, g, state.camera.position) };
	state.triangles[state.trianglesPointer + 3] = (Triangle) { f, g, h, material, getTriangleDist(f, g, h, state.camera.position) };
	state.triangles[state.trianglesPointer + 4] = (Triangle) { a, b, e, material, getTriangleDist(a, b, e, state.camera.position) };
	state.triangles[state.trianglesPointer + 5] = (Triangle) { b, e, f, material, getTriangleDist(b, e, f, state.camera.position) };
	state.triangles[state.trianglesPointer + 6] = (Triangle) { c, d, g, material, getTriangleDist(c, d, g, state.camera.position) };
	state.triangles[state.trianglesPointer + 7] = (Triangle) { d, g, h, material, getTriangleDist(d, g, h, state.camera.position) };
	state.triangles[state.trianglesPointer + 8] = (Triangle) { a, c, e, material, getTriangleDist(a, c, e, state.camera.position) };
	state.triangles[state.trianglesPointer + 9] = (Triangle) { c, e, g, material, getTriangleDist(c, e, g, state.camera.position) };
	state.triangles[state.trianglesPointer + 10] = (Triangle) { b, d, f, material, getTriangleDist(b, d, f, state.camera.position) };
	state.triangles[state.trianglesPointer + 11] = (Triangle) { d, f, h, material, getTriangleDist(d, f, h, state.camera.position) };

	UpdateTriangleData data = (UpdateTriangleData) { 12, buffer_increase };
	return data;
};

f64 getTriangleDist(vf3 a, vf3 b, vf3 c, vf3 camera) {
	vf3 center = (vf3) { (a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3 };
	f64 dist = dist_vf3(center, camera);
	return dist < 0 ? -dist : dist;
}

v3 getChunk(vf3 pos) {
	return (v3) { (i32) (pos.x / CHUNK_SIZE), (i32) (pos.y / CHUNK_SIZE), (i32) (pos.z / CHUNK_SIZE) };
}