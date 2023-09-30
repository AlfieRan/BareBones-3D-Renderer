#ifndef TRIANGULATION_H__
#define TRIANGULATION_H__

#include "types.h"
#include "utils.h"
#include "perspective.h"

void triangles_from_cube(State state, vf3 center, u32 radius, Material material, Triangle* triangles, usize* trianglesPointer, u16* numTriangles);
f64 getTriangleDist(vf3 a, vf3 b, vf3 c, vf3 camera);
v3 getChunk(vf3 pos);

#endif /* TRIANGULATION_H__*/