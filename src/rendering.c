#include "rendering.h"

static bool debug = true;

Barycentric getBarycentric(v2 point, v2 a, v2 b, v2 c) {
	v2 v0 = { b.x - a.x, b.y - a.y };
    v2 v1 = { c.x - a.x, c.y - a.y };
    v2 v2 = { point.x - a.x, point.y - a.y };

    f64 dot00 = v0.x * v0.x + v0.y * v0.y;
    f64 dot01 = v0.x * v1.x + v0.y * v1.y;
    f64 dot02 = v0.x * v2.x + v0.y * v2.y;
    f64 dot11 = v1.x * v1.x + v1.y * v1.y;
    f64 dot12 = v1.x * v2.x + v1.y * v2.y;

    f64 invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    f64 w1 = (dot11 * dot02 - dot01 * dot12) * invDenom;
    f64 w2 = (dot00 * dot12 - dot01 * dot02) * invDenom;
    f64 w3 = 1 - w1 - w2;
	return (Barycentric) { w1, w2, w3 };
}

void verticalLine(State state, int x, int y1, int y2, u32 color) {
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	int index = y1 * SCREEN_WIDTH + x;
	for (int y = y1; y <= y2; y++) {
		state.pixels[index] = color;
        index += SCREEN_WIDTH;
	}
}

void horizontalLine(State state, int y, int x1, int x2, u32 color) {
    if (x1 > x2) {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    u32* start = &state.pixels[y * SCREEN_WIDTH + x1];
    u32* end = &state.pixels[y * SCREEN_WIDTH + x2];
    for (u32* p = start; p <= end; p++) {
        *p = color;
    }
}

u32 alterColourBrightness(u32 colour, f64 factor) {
    // Separate the color components
    u8 alpha = (colour & 0xFF000000) >> 24;
    u8 blue =  (colour & 0x00FF0000) >> 16;
    u8 green = (colour & 0x0000FF00) >> 8;
    u8 red =   (colour & 0x000000FF);

    // Apply the brightness decrease to the blue, green, and red components
	u16 tmp_blue = blue * factor;
	u16 tmp_green = green * factor;
	u16 tmp_red = red * factor;

    blue = min(tmp_blue, 255);
    green = min(tmp_green, 255);
    red = min(tmp_red, 255);

    // Combine the components to form the new color
    return (alpha << 24) | (blue << 16) | (green << 8) | red;
}

void drawLine(State state, vf3 a, vf3 b, u32 color) {
	// Get the positions of the two points on the screen
	ScreenPoint pointA = point_to_screen(state.camera, a);
	ScreenPoint pointB = point_to_screen(state.camera, b);

	if (!pointA.in_front || !pointB.in_front) {
		// If either of the points are behind the camera, don't draw the line
		return;
	}

	v2 posA = pointA.pos;
	v2 posB = pointB.pos;

	// printf("\n[DRAW LINE] posA: {%d, %d} posB: {%d, %d}", posA.x, posA.y, posB.x, posB.y);

	if (posA.x == posB.x) {
		// printf("[\nDRAW LINE] Drawing vertical line");
		// If the x values are the same, just draw a vertical line
		ClampPosition clamped = clamp_position(posA.y, posB.y, 0, SCREEN_HEIGHT - 1);
		for (int y = clamped.low; y <= clamped.high; y++) {
			state.pixels[y * SCREEN_WIDTH + posA.x] = color;
		}

	} else if (posA.y == posB.y) {
		// printf("\n[DRAW LINE] Drawing horizontal line");
		// If the y values are the same, just draw a horizontal line
		ClampPosition clamped = clamp_position(posA.x, posB.x, 0, SCREEN_WIDTH - 1);
		for (int x = clamped.low; x <= clamped.high; x++) {
			state.pixels[posA.y * SCREEN_WIDTH + x] = color;
		}

	} else {
		// printf("\n[DRAW LINE] Drawing non vertical line");
		// Now use y=mx+c to get a definition for the line
		// m = (y_b - y_a)/(x_b - x_a)
		f32 m = (f32)((f32)(posB.y - posA.y) / (f32)(posB.x - posA.x));
		// c = y - mx
		f32 c = (f32)(posA.y - (f32)(m * posA.x));
	
		if (fabs(m) > 1) {
			// If the gradient is greater than 1, we need to loop through the y values
			int x;
			ClampPosition clamped = clamp_position(posA.y, posB.y, 0, SCREEN_HEIGHT - 1);
			for (int y = clamped.low; y <= clamped.high; y++) {
				x = (y - c) / m;
				if (x > 0 && x < SCREEN_WIDTH) {
					state.pixels[y * SCREEN_WIDTH + x] = color;
				}
			}

		} else {
			// Otherwise, we need to loop through the x values
			int y;
			ClampPosition clamped = clamp_position(posA.x, posB.x, 0, SCREEN_WIDTH - 1);
			for (int x = clamped.low; x <= clamped.high; x++) {
				y = (m * x) + c;
				if (y > 0 && y < SCREEN_HEIGHT) {
					state.pixels[y * SCREEN_WIDTH + x] = color;
				}

			}
		};
	}
}


void draw2dCircle(State state, v2 center, u8 radius, u32 colour) {
	// Draw a circle using the midpoint circle algorithm
	// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	int x = radius;
	int y = 0;
	int radiusError = 1 - x;

	while (x >= y) {
		verticalLine(state, center.x + x, center.y - y, center.y + y, colour);
		verticalLine(state, center.x - x, center.y - y, center.y + y, colour);
		verticalLine(state, center.x + y, center.y - x, center.y + x, colour);
		verticalLine(state, center.x - y, center.y - x, center.y + x, colour);

		y++;

		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void drawTriangle(State state, Triangle triangle) {
	LOG("[DRAW TRIANGLES] Getting Points to screen", 3);
	ScreenPoint pointA = point_to_screen(state.camera, triangle.a);
	ScreenPoint pointB = point_to_screen(state.camera, triangle.b);
	ScreenPoint pointC = point_to_screen(state.camera, triangle.c);

	LOG("[DRAW TRIANGLES] Checking if points are in front of camera ", 3);
	if (!pointA.in_front || !pointB.in_front || !pointC.in_front) {
		// If any of the points are behind the camera, don't draw the triangle
		// This isn't ideal as it means that triangles that are partially behind the camera will not be drawn
		// but it prevents triangles from being drawn between positive and negative points which cause visual glitches
		return;
	}

	LOG("[DRAW TRIANGLES] Getting distances", 3);
	// Colour calculation - currently sets the whole triangle to one colour but ideally would be a gradient
	f32 distA = dist_vf3(triangle.material.closest_light, triangle.a);
	f32 distB = dist_vf3(triangle.material.closest_light, triangle.b);
	f32 distC = dist_vf3(triangle.material.closest_light, triangle.c);
	f32 avgDist = avg3(distA, distB, distC);

	LOG("[DRAW TRIANGLES] Calculating colour", 3);
	f64 colourFalloff = min((triangle.material.colour_falloff / avgDist), 1);
	u32 colour = alterColourBrightness(triangle.material.colour, colourFalloff);

	v2 posA = pointA.pos; v2 posB = pointB.pos; v2 posC = pointC.pos;

	LOG("[DRAW TRIANGLES] Calculting Bounds", 3);
	// Get bounding Rectangle within the screen
	i32 minX = clamp(min3(posA.x, posB.x, posC.x), 0, SCREEN_WIDTH - 1);
	i32 maxX = clamp(max3(posA.x, posB.x, posC.x), 0, SCREEN_WIDTH - 1);
	i32 minY = clamp(min3(posA.y, posB.y, posC.y), 0, SCREEN_HEIGHT - 1);
	i32 maxY = clamp(max3(posA.y, posB.y, posC.y), 0, SCREEN_HEIGHT - 1);

	// loop through the bounding rectangle
	LOG("[DRAW TRIANGLES] Drawing", 3);
	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			// Check that half lines are all positive or all negative
			LOG("[DRAW TRIANGLES] Calculating Half Lines", 3);
			i32 h1 = (posA.x - posB.x) * (y - posA.y) - (posA.y - posB.y) * (x - posA.x);
			i32 h2 = (posB.x - posC.x) * (y - posB.y) - (posB.y - posC.y) * (x - posB.x);
			i32 h3 = (posC.x - posA.x) * (y - posC.y) - (posC.y - posA.y) * (x - posC.x);

			// printf("\n[DRAW TRIANGLE] pos(%d, %d) h(%d, %d, %d)", x, y, h1, h2, h3);

			// LOG("[DRAW TRIANGLES] Checking Half Lines", 3);
			if ((bool)((h1 > 0 && h2 > 0 && h3 > 0) || (h1 < 0 && h2 < 0 && h3 < 0))) {
				// If they are, draw the pixel
				LOG("[DRAW TRIANGLES] Drawing Pixel", 3);
				state.pixels[y * SCREEN_WIDTH + x] = colour;
			}
		}
	}

	if (debug) {
		LOG("[DRAW TRIANGLES] Drawing Debug Points", 3);
		if (insideScreen(posA)) {draw2dCircle(state, posA, 3, RED);}
		if (insideScreen(posB)) {draw2dCircle(state, posB, 3, RED);}
		if (insideScreen(posC)) {draw2dCircle(state, posC, 3, RED);}
	}
}

bool insideScreen(v2 pos) {
	return pos.x > 0 && pos.x < SCREEN_WIDTH && pos.y > 0 && pos.y < SCREEN_HEIGHT;
}

int compareTriangles(const void* A, const void* B) {
	Triangle triA = *(const Triangle*) A;
	Triangle triB = *(const Triangle*) B;

	if (triA.dist < triB.dist) {
		return -1;
	} else if (triA.dist > triB.dist) {
		return 1;
	} else {
		return 0;
	}
}

TriangleBoundaries getTriangleBoundaries(State state, Triangle triangle) {
	ScreenPoint pointA = point_to_screen(state.camera, triangle.a);
	ScreenPoint pointB = point_to_screen(state.camera, triangle.b);
	ScreenPoint pointC = point_to_screen(state.camera, triangle.c);

	if (!pointA.in_front || !pointB.in_front || !pointC.in_front) {
		ScreenPoint tmp = (ScreenPoint) { (v2) { 0, 0 }, false, 0 };
		return (TriangleBoundaries) { 0, 0, 0, 0, tmp, tmp, tmp };
	}

	u32 minX = min3(pointA.pos.x, pointB.pos.x, pointC.pos.x);
	u32 maxX = max3(pointA.pos.x, pointB.pos.x, pointC.pos.x);
	u32 minY = min3(pointA.pos.y, pointB.pos.y, pointC.pos.y);
	u32 maxY = max3(pointA.pos.y, pointB.pos.y, pointC.pos.y);

	return (TriangleBoundaries) { minX, maxX, minY, maxY, pointA, pointB, pointC };
}

void drawTriangles(State state, Triangle* triangles, usize trianglePointer) {
	// Setup the depth buffer
	float depthBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		depthBuffer[i] = 1000000;
	}

	// Now draw the triangles
	for (usize i = 0; i < trianglePointer; i++) {
		// LOG("[DRAW TRIANGLES] Drawing Triangle", 3);
		Triangle triangle = triangles[i];
		TriangleBoundaries boundaries = getTriangleBoundaries(state, triangle);
		if (boundaries.max_x == 0 && boundaries.max_y == 0 && boundaries.min_x == 0 && boundaries.min_y == 0) {
			continue;
		} else if (!boundaries.a.in_front && !boundaries.b.in_front && !boundaries.c.in_front) {
			continue;
		}

		ScreenPoint a = boundaries.a;
		ScreenPoint b = boundaries.b;
		ScreenPoint c = boundaries.c;
		
		// Barycentric precalculations
		v2 bar_v0 = { b.pos.x - a.pos.x, b.pos.y - a.pos.y };
    	v2 bar_v1 = { c.pos.x - a.pos.x, c.pos.y - a.pos.y };
		f64 bar_dot00 = bar_v0.x * bar_v0.x + bar_v0.y * bar_v0.y;
		f64 bar_dot01 = bar_v0.x * bar_v1.x + bar_v0.y * bar_v1.y;
		f64 bar_dot11 = bar_v1.x * bar_v1.x + bar_v1.y * bar_v1.y;
		f64 bar_invDenom = 1 / (bar_dot00 * bar_dot11 - bar_dot01 * bar_dot01);

		// Loop through the bounding rectangle
		// LOG("[DRAW TRIANGLES] Drawing", 3);
		
		u32 boundary_x_width = boundaries.max_x - boundaries.min_x;
		u32 boundary_y_width = boundaries.max_y - boundaries.min_y;
		u32 boundary_area = boundary_x_width * boundary_y_width;
		// printf("\n[DRAW TRIANGLES] Triangle %d/%d - Area: %d", i, trianglePointer, boundary_area);

		if (boundary_area > 100000) {
			// printf("\n[DRAW TRIANGLES] Triangle %d/%d - Area: %d", i, trianglePointer, boundary_area);
			continue;
		}

		for (u32 x = boundaries.min_x; x <= boundaries.max_x; x++) {
			for (u32 y = boundaries.min_y; y <= boundaries.max_y; y++) {
				// check if the point is inside the screen
				if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
					continue;
				}
				
				// finish barycentric calculations
				v2 bar_v2 = { x - a.pos.x, y - a.pos.y };
				f64 bar_dot02 = bar_v0.x * bar_v2.x + bar_v0.y * bar_v2.y;
				f64 bar_dot12 = bar_v1.x * bar_v2.x + bar_v1.y * bar_v2.y;
				f64 bar_w1 = (bar_dot11 * bar_dot02 - bar_dot01 * bar_dot12) * bar_invDenom;
				f64 bar_w2 = (bar_dot00 * bar_dot12 - bar_dot01 * bar_dot02) * bar_invDenom;
				f64 bar_w3 = 1 - bar_w1 - bar_w2;

				// Check if the point is inside the triangle
				if ((bool)((bar_w1 > 0 && bar_w2 > 0 && bar_w3 > 0) || (bar_w1 < 0 && bar_w2 < 0 && bar_w3 < 0))) {
					// If it is, calculate the depth
					f64 depth = (bar_w1 * a.depth) + (bar_w2 * b.depth) + (bar_w3 * c.depth);
					// Check if the depth is less than the depth buffer
					if (depth < depthBuffer[y * SCREEN_WIDTH + x]) {
						// LOG("[DRAW TRIANGLES] Drawing Pixel", 3);
						// If it is, draw the pixel

						// colour calculation - currently based on camera being the light source
						// f64 colourFalloff = min((triangle.material.colour_falloff / depth), 1);
						// u32 colour = alterColourBrightness(triangle.material.colour, colourFalloff);
						u32 colour = triangle.material.colour;

						// draw the pixel
						state.pixels[y * SCREEN_WIDTH + x] = colour;
						depthBuffer[y * SCREEN_WIDTH + x] = depth;
					}
				}
			}
		}

		// drawTriangle(state, triangle);
		// LOG("[DRAW TRIANGLES] Drew Triangle", 3);
		// printf("\n[DRAW TRIANGLES] Triangle %d/%d", i, trianglePointer);
	}
	LOG("[DRAW TRIANGLES] Triangles drawn", 2);
}

void drawCubeMeshFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, vf3 e, vf3 f, vf3 g, vf3 h, u32 color) {
	drawSquareMeshFromPoints(state, a, b, c, d, color);
	drawSquareMeshFromPoints(state, e, f, g, h, color);
	drawLine(state, a, e, color);
	drawLine(state, b, f, color);
	drawLine(state, c, g, color);
	drawLine(state, d, h, color);
}

void drawSquareMeshFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color) {
	drawLine(state, a, b, color);
	drawLine(state, a, c, color);
	drawLine(state, b, d, color);
	drawLine(state, c, d, color);
}

void drawSquareMesh(State state, vf3 center, u32 length) {
	u32 h = length / 2;
	drawSquareMeshFromPoints(
		state,
		(vf3) { center.x - h, center.y, center.z + h },
		(vf3) { center.x + h, center.y, center.z + h },
		(vf3) { center.x - h, center.y, center.z - h },
		(vf3) { center.x + h, center.y, center.z - h },
		0xFFFF00AA
	);
}

void drawCubeMesh(State state, vf3 center, u32 length, u32 color) {
	u32 h = length / 2;
	drawCubeMeshFromPoints(
		state,
		(vf3) { center.x - h, center.y - h, center.z + h },
		(vf3) { center.x - h, center.y + h, center.z + h },
		(vf3) { center.x - h, center.y - h, center.z - h },
		(vf3) { center.x - h, center.y + h, center.z - h },
		(vf3) { center.x + h, center.y - h, center.z + h },
		(vf3) { center.x + h, center.y + h, center.z + h },
		(vf3) { center.x + h, center.y - h, center.z - h },
		(vf3) { center.x + h, center.y + h, center.z - h },
		color
	);
} 

void drawSquareFromPoints(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color) {
	LOG("Assigning Square Material", 3);
	Material material = (Material) { color, state.camera.position, 100 };
	LOG("Drawing Triangles", 3);
	drawTriangle(state, (Triangle) { a, b, c, material, getTriangleDist(a,b,c,state.camera.position), true });
	drawTriangle(state, (Triangle) { b, c, d, material, getTriangleDist(b,c,d,state.camera.position), true });
}

void drawSquare(State state, vf3 center, u32 length, u32 color) {
	u32 h_length = length / 2;
	vf3 a = (vf3) { center.x - h_length, center.y - h_length, center.z };
	vf3 b = (vf3) { center.x - h_length, center.y + h_length, center.z };
	vf3 c = (vf3) { center.x + h_length, center.y - h_length, center.z };
	vf3 d = (vf3) { center.x + h_length, center.y + h_length, center.z };

	drawSquareFromPoints(state, a, b, c, d, color);
}

void drawCube(State state, vf3 center, u32 length, u32 color) {
	LOG("Assigning points", 3);
	u32 h_length = length / 2;
	vf3 a = (vf3) { center.x - h_length, center.y - h_length, center.z - h_length };
	vf3 b = (vf3) { center.x - h_length, center.y - h_length, center.z + h_length };
	vf3 c = (vf3) { center.x - h_length, center.y + h_length, center.z - h_length };
	vf3 d = (vf3) { center.x - h_length, center.y + h_length, center.z + h_length };
	vf3 e = (vf3) { center.x + h_length, center.y - h_length, center.z - h_length };
	vf3 f = (vf3) { center.x + h_length, center.y - h_length, center.z + h_length };
	vf3 g = (vf3) { center.x + h_length, center.y + h_length, center.z - h_length };
	vf3 h = (vf3) { center.x + h_length, center.y + h_length, center.z + h_length };

	LOG("Drawing squares", 3);
	drawSquareFromPoints(state, a, b, c, d, color);
	drawSquareFromPoints(state, e, f, g, h, color);
	drawSquareFromPoints(state, a, b, e, f, color);
	drawSquareFromPoints(state, c, d, g, h, color);
	drawSquareFromPoints(state, a, c, e, g, color);
	drawSquareFromPoints(state, b, d, f, h, color);
}
