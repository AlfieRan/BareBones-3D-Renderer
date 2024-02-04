#ifndef UTILS_H__
#define UTILS_H__

#include "types.h"

#define abs(x) ((x) < 0 ? -(x) : (x));
#define min(a,b) (((a) < (b)) ? (a) : (b));
#define max(a,b) (((a) > (b)) ? (a) : (b));
#define avg(a,b) (((a) + (b)) / 2);
#define avg3(a,b,c) (((a) + (b) + (c)) / 3);

int clamp(int value, int minimum, int maximum);

void LOG(char *message, u8 level);
void setLoggingLevel(u8 level);

i32 f64_to_i32(f64 value);
bool is_near_zero(f64 value, f64 epsilon);

i32 min3(i32 a, i32 b, i32 c);
i32 max3(i32 a, i32 b, i32 c);
u32 min3u(u32 a, u32 b, u32 c);
u32 max3u(u32 a, u32 b, u32 c);

f64 dot_vf2(vf2 a, vf2 b);
f64 det_vf2(vf2 a, vf2 b);
f64 product_vf2(vf2 a);
i32 dot_v2(v2 a, v2 b);
i32 product_v2(v2 a);

f32 dot_vf3(vf3 a, vf3 b);
f32 product_vf3(vf3 a);
f32 dist_vf3(vf3 a, vf3 b);
f64 sqr_dist_vf3(vf3 a, vf3 b);

m3 matrixMult_m3(m3 a, m3 b);
vf3 matrixMult_vf3(m3 a, v3 b);

// Bitmap stuff
BitMap* getBitMap();
char* getBitMapItem(State state, u8 num);

#endif /* UTILS_H__*/