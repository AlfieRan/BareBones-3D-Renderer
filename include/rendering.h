#ifndef RENDERING_H__
#define RENDERING_H__

#include "types.h"
#include "utils.h"
#include "perspective.h"
#include "triangulation.h"

// Basic 2D Drawing Functions
void verticalLine(State state, int x, int y1, int y2, u32 color);
void horizontalLine(State state, int y, int x1, int x2, u32 color);

// Core 3D Drawing Functions
void drawLine(State state, vf3 a, vf3 b, u32 color);
void drawScreenLine(State state, v2 a, v2 b, u32 color);
void drawTriangle(State state, Triangle triangle);
void drawTriangles(State state);
int compareTriangles(const void* A, const void* B);

// Helper Functions
void drawCubeMeshFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, vf3 e, vf3 f, vf3 g, vf3 h, u32 color);
void drawSquareMeshFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color);
void drawSquareFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color);

void drawSquareMesh(State state, vf3 center, u32 length);
void drawCubeMesh(State state, vf3 center, u32 length, u32 color);
void drawSquareFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color);

void drawSquare(State state, vf3 center, u32 length, u32 color);
void drawCube(State state, vf3 center, u32 length, u32 color);

// Testing Functions



#endif /* RENDERING_H__*/