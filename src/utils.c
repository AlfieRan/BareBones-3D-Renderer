#include "utils.h"
#include "types.h"

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