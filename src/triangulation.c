#include "triangulation.h"

void triangles_from_cube(State state, vf3 center, u32 radius, Material material, Triangle** triangles, usize* trianglesPointer, u16* numTriangles) {
	f64 sqr_dist = sqr_dist_vf3(center, state.camera.position);
	if (sqr_dist > MAX_VIEW_DISTANCE) {
		return;
	} else if (sqr_dist > 50000) {
		printf("Sqr dist: %f\n", sqr_dist);
	}

	LOG("[Cube] Marking cube points", 3);
	vf3 a = (vf3) { center.x - radius, center.y - radius, center.z - radius };
	vf3 b = (vf3) { center.x - radius, center.y - radius, center.z + radius };
	vf3 c = (vf3) { center.x - radius, center.y + radius, center.z - radius };
	vf3 d = (vf3) { center.x - radius, center.y + radius, center.z + radius };
	vf3 e = (vf3) { center.x + radius, center.y - radius, center.z - radius };
	vf3 f = (vf3) { center.x + radius, center.y - radius, center.z + radius };
	vf3 g = (vf3) { center.x + radius, center.y + radius, center.z - radius };
	vf3 h = (vf3) { center.x + radius, center.y + radius, center.z + radius };

	// if (*trianglesPointer + 12 >= *numTriangles) {
	// 	*numTriangles += TRIANGLE_BUFFER_SIZE;
	// 	triangles = realloc(triangles, sizeof(Triangle) * *numTriangles);
	// }

	if (*trianglesPointer + 12 >= *numTriangles) {
		u16 newNumTriangles = *numTriangles + TRIANGLE_BUFFER_SIZE;
        Triangle* newTriangles = (Triangle *)realloc(*triangles, sizeof(Triangle) * newNumTriangles);
		
		// LOG("[Cube] Checking if new array is null\n", 0);
		if (newTriangles == NULL) {
			LOG("Failed to allocate memory for triangles\n", 0);
			exit(1);
		}

		*triangles = newTriangles;
        *numTriangles = newNumTriangles;
    }

	LOG("[Cube] Allocating triangles to buffer (0/12)", 3);
	(*triangles)[*trianglesPointer] = (Triangle) { a, b, c, material, getTriangleSqrDist(a, b, c, state.camera.position), false };
	(*triangles)[*trianglesPointer + 1] = (Triangle) { b, c, d, material, getTriangleSqrDist(b, c, d, state.camera.position), false };
	(*triangles)[*trianglesPointer + 2] = (Triangle) { e, f, g, material, getTriangleSqrDist(e, f, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 3] = (Triangle) { f, g, h, material, getTriangleSqrDist(f, g, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (4/12)", 3);
	(*triangles)[*trianglesPointer + 4] = (Triangle) { a, b, e, material, getTriangleSqrDist(a, b, e, state.camera.position), false };
	(*triangles)[*trianglesPointer + 5] = (Triangle) { b, e, f, material, getTriangleSqrDist(b, e, f, state.camera.position), false };
	(*triangles)[*trianglesPointer + 6] = (Triangle) { c, d, g, material, getTriangleSqrDist(c, d, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 7] = (Triangle) { d, g, h, material, getTriangleSqrDist(d, g, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (8/12)", 3);
	(*triangles)[*trianglesPointer + 8] = (Triangle) { a, c, e, material, getTriangleSqrDist(a, c, e, state.camera.position), false };
	(*triangles)[*trianglesPointer + 9] = (Triangle) { c, e, g, material, getTriangleSqrDist(c, e, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 10] = (Triangle) { b, d, f, material, getTriangleSqrDist(b, d, f, state.camera.position), false };
	(*triangles)[*trianglesPointer + 11] = (Triangle) { d, f, h, material, getTriangleSqrDist(d, f, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (12/12)", 3);

	*trianglesPointer += 12;
	LOG("[Cube] Increasing triangle buffer pointer", 3);
};

void triangles_from_plane(State state, vf3 center, u32 radius, Material material, Triangle** triangles, usize* trianglesPointer, u16* numTriangles) {
	f64 sqr_dist = sqr_dist_vf3(center, state.camera.position);
	if (sqr_dist > MAX_VIEW_DISTANCE) {
		return;
	}

	LOG("[Plane] Marking plane points", 3);
	vf3 a = (vf3) { center.x - radius, center.y, center.z - radius };
	vf3 b = (vf3) { center.x - radius, center.y, center.z + radius };
	vf3 c = (vf3) { center.x + radius, center.y, center.z - radius };
	vf3 d = (vf3) { center.x + radius, center.y, center.z + radius };

	if (*trianglesPointer + 2 >= *numTriangles) {
		u16 newNumTriangles = *numTriangles + TRIANGLE_BUFFER_SIZE;
		Triangle* newTriangles = (Triangle *)realloc(*triangles, sizeof(Triangle) * newNumTriangles);
		
		// LOG("[Cube] Checking if new array is null\n", 0);
		if (newTriangles == NULL) {
			LOG("Failed to allocate memory for triangles\n", 0);
			exit(1);
		}

		*triangles = newTriangles;
		*numTriangles = newNumTriangles;
	}

	LOG("[Plane] Allocating triangles to buffer (0/2)", 3);
	(*triangles)[*trianglesPointer] = (Triangle) { a, b, c, material, getTriangleSqrDist(a, b, c, state.camera.position), false };
	(*triangles)[*trianglesPointer + 1] = (Triangle) { b, c, d, material, getTriangleSqrDist(b, c, d, state.camera.position), false };
	LOG("[Plane] Allocating triangles to buffer (2/2)", 3);

	*trianglesPointer += 2;
	LOG("[Plane] Increasing triangle buffer pointer", 3);
};


void debug_cube_triangles(State state, vf3 center, u32 radius, Triangle** triangles, usize* trianglesPointer, u16* numTriangles) {
	f64 sqr_dist = sqr_dist_vf3(center, state.camera.position);
	if (sqr_dist > MAX_VIEW_DISTANCE) {
		return;
	}

	LOG("[Cube] Marking cube points", 3);
	vf3 a = (vf3) { center.x - radius, center.y - radius, center.z - radius };
	vf3 b = (vf3) { center.x - radius, center.y - radius, center.z + radius };
	vf3 c = (vf3) { center.x - radius, center.y + radius, center.z - radius };
	vf3 d = (vf3) { center.x - radius, center.y + radius, center.z + radius };
	vf3 e = (vf3) { center.x + radius, center.y - radius, center.z - radius };
	vf3 f = (vf3) { center.x + radius, center.y - radius, center.z + radius };
	vf3 g = (vf3) { center.x + radius, center.y + radius, center.z - radius };
	vf3 h = (vf3) { center.x + radius, center.y + radius, center.z + radius };

	if (*trianglesPointer + 12 >= *numTriangles) {
		u16 newNumTriangles = *numTriangles + TRIANGLE_BUFFER_SIZE;
        Triangle* newTriangles = (Triangle *)realloc(*triangles, sizeof(Triangle) * newNumTriangles);
		
		// LOG("[Cube] Checking if new array is null\n", 0);
		if (newTriangles == NULL) {
			LOG("Failed to allocate memory for triangles\n", 0);
			exit(1);
		}

		*triangles = newTriangles;
        *numTriangles = newNumTriangles;
    }

	Material Green = (Material) { GREEN, state.camera.position, 10000 };
	Material Purple = (Material) { PURPLE, state.camera.position, 10000 };
	Material Red = (Material) { RED, state.camera.position, 10000 };
	Material Blue = (Material) { BLUE, state.camera.position, 10000 };

	LOG("[Cube] Allocating triangles to buffer (0/12)", 3);
	(*triangles)[*trianglesPointer] = (Triangle) { a, b, c, Green, getTriangleSqrDist(a, b, c, state.camera.position), false };
	(*triangles)[*trianglesPointer + 1] = (Triangle) { b, c, d, Purple, getTriangleSqrDist(b, c, d, state.camera.position), false };
	(*triangles)[*trianglesPointer + 2] = (Triangle) { e, f, g, Red, getTriangleSqrDist(e, f, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 3] = (Triangle) { f, g, h, Blue, getTriangleSqrDist(f, g, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (4/12)", 3);
	(*triangles)[*trianglesPointer + 4] = (Triangle) { a, b, e, Blue, getTriangleSqrDist(a, b, e, state.camera.position), false };
	(*triangles)[*trianglesPointer + 5] = (Triangle) { b, e, f, Red, getTriangleSqrDist(b, e, f, state.camera.position), false };
	(*triangles)[*trianglesPointer + 6] = (Triangle) { c, d, g, Purple, getTriangleSqrDist(c, d, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 7] = (Triangle) { d, g, h, Green, getTriangleSqrDist(d, g, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (8/12)", 3);
	(*triangles)[*trianglesPointer + 8] = (Triangle) { a, c, e, Green, getTriangleSqrDist(a, c, e, state.camera.position), false };
	(*triangles)[*trianglesPointer + 9] = (Triangle) { c, e, g, Purple, getTriangleSqrDist(c, e, g, state.camera.position), false };
	(*triangles)[*trianglesPointer + 10] = (Triangle) { b, d, f, Red, getTriangleSqrDist(b, d, f, state.camera.position), false };
	(*triangles)[*trianglesPointer + 11] = (Triangle) { d, f, h, Blue, getTriangleSqrDist(d, f, h, state.camera.position), false };
	LOG("[Cube] Allocating triangles to buffer (12/12)", 3);

	*trianglesPointer += 12;
	LOG("[Cube] Increasing triangle buffer pointer", 3);
};

f64 getTriangleSqrDist(vf3 a, vf3 b, vf3 c, vf3 camera) {
	vf3 center = (vf3) { (a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3 };
	f64 sqr_dist = sqr_dist_vf3(center, camera);
	return sqr_dist;
}

v3 getChunk(vf3 pos) {
	return (v3) { (i32) (pos.x / CHUNK_SIZE), (i32) (pos.y / CHUNK_SIZE), (i32) (pos.z / CHUNK_SIZE) };
}