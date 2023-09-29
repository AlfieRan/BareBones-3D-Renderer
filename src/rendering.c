#include "rendering.h"

void verticalLine(State state, int x, int y1, int y2, u32 color) {
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int y = y1; y <= y2; y++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

void horizontalLine(State state, int y, int x1, int x2, u32 color) {
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	for (int x = x1; x <= x2; x++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
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

void drawTriangle(State state, Triangle triangle) {
	ScreenPoint pointA = point_to_screen(state.camera, triangle.a);
	ScreenPoint pointB = point_to_screen(state.camera, triangle.b);
	ScreenPoint pointC = point_to_screen(state.camera, triangle.c);

	if (!pointA.in_front || !pointB.in_front || !pointC.in_front) {
		// If any of the points are behind the camera, don't draw the triangle
		// This isn't ideal as it means that triangles that are partially behind the camera will not be drawn
		// but it prevents triangles from being drawn between positive and negative points which cause visual glitches
		return;
	}

	v2 posA = pointA.pos; v2 posB = pointB.pos; v2 posC = pointC.pos;

	// Get bounding Rectangle within the screen
	i32 minX = clamp(min3(posA.x, posB.x, posC.x), 0, SCREEN_WIDTH - 1);
	i32 maxX = clamp(max3(posA.x, posB.x, posC.x), 0, SCREEN_WIDTH - 1);
	i32 minY = clamp(min3(posA.y, posB.y, posC.y), 0, SCREEN_HEIGHT - 1);
	i32 maxY = clamp(max3(posA.y, posB.y, posC.y), 0, SCREEN_HEIGHT - 1);

	// loop through the bounding rectangle
	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			// Check that half lines are all positive or all negative
			i32 h1 = (posA.x - posB.x) * (y - posA.y) - (posA.y - posB.y) * (x - posA.x);
			i32 h2 = (posB.x - posC.x) * (y - posB.y) - (posB.y - posC.y) * (x - posB.x);
			i32 h3 = (posC.x - posA.x) * (y - posC.y) - (posC.y - posA.y) * (x - posC.x);

			// printf("\n[DRAW TRIANGLE] pos(%d, %d) h(%d, %d, %d)", x, y, h1, h2, h3);

			if ((h1 > 0 && h2 > 0 && h3 > 0) || (h1 < 0 && h2 < 0 && h3 < 0)) {
				// If they are, draw the pixel
				state.pixels[y * SCREEN_WIDTH + x] = triangle.color;
			}
		}
	}
}

void drawEmptyCube(State state, vf3 a, vf3 b, vf3 c, vf3 d, vf3 e, vf3 f, vf3 g, vf3 h, u32 color) {
	drawEmptySquare(state, a, b, c, d, color);
	drawEmptySquare(state, e, f, g, h, color);
	drawLine(state, a, e, color);
	drawLine(state, b, f, color);
	drawLine(state, c, g, color);
	drawLine(state, d, h, color);
}

void drawEmptySquare(State state, vf3 a, vf3 b, vf3 c, vf3 d, u32 color) {
	drawLine(state, a, b, color);
	drawLine(state, a, c, color);
	drawLine(state, b, d, color);
	drawLine(state, c, d, color);
}

void drawTestSquare(State state, vf3 center, u32 length) {
	u32 h = length / 2;
	drawEmptySquare(
		state,
		(vf3) { center.x - h, center.y, center.z + h },
		(vf3) { center.x + h, center.y, center.z + h },
		(vf3) { center.x - h, center.y, center.z - h },
		(vf3) { center.x + h, center.y, center.z - h },
		0xFFFF00AA
	);
}

void drawTestCube(State state, vf3 center, u32 length, u32 color) {
	u32 h = length / 2;
	drawEmptyCube(
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
	drawTriangle(state, (Triangle) { a, b, c, color });
	drawTriangle(state, (Triangle) { b, c, d, color });
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
	u32 h_length = length / 2;
	vf3 a = (vf3) { center.x - h_length, center.y - h_length, center.z - h_length };
	vf3 b = (vf3) { center.x - h_length, center.y - h_length, center.z + h_length };
	vf3 c = (vf3) { center.x - h_length, center.y + h_length, center.z - h_length };
	vf3 d = (vf3) { center.x - h_length, center.y + h_length, center.z + h_length };
	vf3 e = (vf3) { center.x + h_length, center.y - h_length, center.z - h_length };
	vf3 f = (vf3) { center.x + h_length, center.y - h_length, center.z + h_length };
	vf3 g = (vf3) { center.x + h_length, center.y + h_length, center.z - h_length };
	vf3 h = (vf3) { center.x + h_length, center.y + h_length, center.z + h_length };

	drawSquareFromPoints(state, a, b, c, d, color);
	drawSquareFromPoints(state, e, f, g, h, color);
	drawSquareFromPoints(state, a, b, e, f, color);
	drawSquareFromPoints(state, c, d, g, h, color);
	drawSquareFromPoints(state, a, c, e, g, color);
	drawSquareFromPoints(state, b, d, f, h, color);
}
