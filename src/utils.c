#include "utils.h"
#include "types.h"

i32 min3(i32 a, i32 b, i32 c) {
	i32 minAB = min(a, b);
	return min(minAB, c);
}

i32 max3(i32 a, i32 b, i32 c) {
	i32 maxAB = max(a, b);
	return max(maxAB, c);
}

f64 dot_vf2(vf2 a, vf2 b) {
	return (f64)((f64)(a.x * b.x) + (f64)(a.y * b.y));
}

f64 det_vf2(vf2 a, vf2 b) {
	return (f64)((f64)(a.x * b.y) - (f64)(a.y * b.x));
}

f64 product_vf2(vf2 a) {
	return (f64)(
		sqrt((f64)(
			(f64)(a.x * a.x) + (f64)(a.y * a.y)
		))
	);
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
	return sqrt((f64)((f64)(a.x * a.x) + (f64)(a.y * a.y) + (f64)(a.z * a.z)));
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

char* getNumberBitMap(u8 num) {
	if (num > 9) return NULL;
	FILE *fp = fopen("./assets/num_chars.txt", "r");

	if (fp == NULL) {
        printf("Error: could not open the num_chars.txt file\n");
        exit(1);
    }

	// Skip to the required line number in the file
	u8 lineNumber = (num * 2) + 1;
    char line[49];
    for (int i = 0; i < lineNumber; i++) {
        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("Invalid line number.\n");
            fclose(fp);
            return NULL;
        }
    }

    // Close the file
    fclose(fp);

    // Remove newline character at the end of the line
    line[strcspn(line, "\n")] = '\0';
	return strdup(line);
}

int clamp(int value, int minimum, int maximum) {
	if (value < minimum) {
		return minimum;
	} else if (value > maximum) {
		return maximum;
	}

	return value;
}