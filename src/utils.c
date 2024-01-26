#include "utils.h"
#include "types.h"

// Logging
static u8 logging = 1;
// 0 - Only Key Messages
// 1 - Key Messages and Info Messages
// 2 - Every Frame Primary Messages
// 3 - Every Frame Secondary Messages
// 4 - All Messages



void LOG(char *message, u8 level) {
	if (logging > level) {
		printf("\n%s", message);
	}
}

void setLoggingLevel(u8 level) {
	logging = level;
}


i32 min3(i32 a, i32 b, i32 c) {
	i32 minAB = min(a, b);
	return min(minAB, c);
}

i32 max3(i32 a, i32 b, i32 c) {
	i32 maxAB = max(a, b);
	return max(maxAB, c);
}

u32 min3u(u32 a, u32 b, u32 c) {
	u32 minAB = min(a, b);
	return min(minAB, c);
}

u32 max3u(u32 a, u32 b, u32 c) {
	u32 maxAB = max(a, b);
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

f32 dist_vf3(vf3 a, vf3 b) {
	return product_vf3((vf3){ a.x - b.x, a.y - b.y, a.z - b.z });
}

f64 sqr_dist_vf3(vf3 a, vf3 b) {
	return (f64)((f32)(a.x - b.x) * (f32)(a.x - b.x) + (f32)(a.y - b.y) * (f32)(a.y - b.y) + (f32)(a.z - b.z) * (f32)(a.z - b.z));
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

BitMap* getBitMap() {
	FILE *fp = fopen("./assets/bitmap.txt", "r");

	if (fp == NULL) {
        LOG("Error: could not open the bitmap.txt file", 0);
        exit(1);
    };

	BitMap* lines = malloc(sizeof(BitMap));
	if (lines == NULL) {
        LOG("Error: memory allocation for BitMap failed", 0);
        fclose(fp);
        exit(1);
    }

	char line[BITS_PER_CHAR + 2];
	for (int i = 0; i < NUM_CHARS; i++) {
		if (fgets(line, sizeof(line), fp) == NULL) {
			LOG("[ERROR] Can't get bit map, invalid number of lines.", 1);
			fclose(fp);
			free(lines);
			return NULL;
		}
			
		line[strcspn(line, "\n")] = '\0';
		if (strlen(line) > BITS_PER_CHAR) {
            LOG("[ERROR] Line length exceeds BITS_PER_CHAR.", 1);
            fclose(fp);
            free(lines);
            return NULL;
        }
		
		strncpy((*lines)[i], line, BITS_PER_CHAR);
        (*lines)[i][BITS_PER_CHAR] = '\0'; // Ensure null termination
	}

	fclose(fp);
	return lines;
}


char* getBitMapItem(State state, u8 num) {
	if (state.bitmap == NULL) {
		LOG("[ERROR] Can't get bit map item, bitmap is null.", 1);
		return NULL;
	}

	return strdup((*state.bitmap)[num]);
}

int clamp(int value, int minimum, int maximum) {
	if (value < minimum) {
		return minimum;
	} else if (value > maximum) {
		return maximum;
	}

	return value;
}