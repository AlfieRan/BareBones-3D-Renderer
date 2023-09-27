#ifndef UTILS_H__
#define UTILS_H__

#include "types.h"

#define abs(x) ((x) < 0 ? -(x) : (x));
#define min(a,b) (((a) < (b)) ? (a) : (b));
#define max(a,b) (((a) > (b)) ? (a) : (b));

f64 dot_vf2(vf2 a, vf2 b);
f64 det_vf2(vf2 a, vf2 b);
f64 product_vf2(vf2 a);
i32 dot_v2(v2 a, v2 b);
i32 product_v2(v2 a);
f32 dot_vf3(vf3 a, vf3 b);
f32 product_vf3(vf3 a);
m3 matrixMult_m3(m3 a, m3 b);
vf3 matrixMult_vf3(m3 a, v3 b);
char* getNumberBitMap(u8 num);

#endif /* UTILS_H__*/