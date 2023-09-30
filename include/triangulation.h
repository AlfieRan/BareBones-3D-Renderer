#ifndef TRIANGULATION_H__
#define TRIANGULATION_H__

#include "types.h"
#include "utils.h"
#include "perspective.h"

void triangles_from_cube(vf3 center, u32 radius, Material material, Triangle* triangles, usize* trianglesPointer, u16* numTriangles);

#endif /* TRIANGULATION_H__*/