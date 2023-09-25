#include "utils.h"
#include "types.h"

f32 dot_vf2(vf2 a, vf2 b) {
	return (f32)((f32)(a.x * b.x) + (f32)(a.y * b.y));
}

f32 product_vf2(vf2 a) {
	return (f32)(sqrt((f32)((f32)(a.x * a.x) + (f32)(a.y * a.y))));
}

i32 dot_v2(v2 a, v2 b) {
	return a.x * b.x + a.y * b.y;
}

i32 product_v2(v2 a) {
	return sqrt(a.x * a.x + a.y * a.y);
}

f32 dot_vf3(vf3 a, vf3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 product_vf3(vf3 a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

m3 matrixMult_m3(m3 a, m3 b){
	return (m3) {
		a.a * b.a + a.b * b.d + a.c * b.g,
		a.a * a.b + a.b * b.e + a.c * b.h,
		a.a * a.c + a.b * b.f + a.c * b.i,
		a.d * b.a + a.e * b.d + a.f * b.g,
		a.d * a.b + a.e * b.e + a.f * b.h,
		a.d * a.c + a.e * b.f + a.f * b.i,
		a.g * b.a + a.h * b.d + a.i * b.g,
		a.g * a.b + a.h * b.e + a.i * b.h,
		a.g * a.c + a.h * b.f + a.i * b.i
	};
}

vf3 matrixMult_vf3(m3 a, v3 b){
	return (vf3) {
		a.a * b.x + a.b * b.y + a.c * b.z,
		a.d * b.x + a.e * b.y + a.f * b.z,
		a.g * b.x + a.h * b.y + a.i * b.z
	};
}

i32 distance_between_points(v3 a, v3 b) {
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2));
}

bool insideScreen(v2 pos, Camera camera) {
	return pos.x > 0 && pos.x < camera.screen.horizontal && pos.y > 0 && pos.y < camera.screen.vertical;
}