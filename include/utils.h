#ifndef UTILS_H__
#define UTILS_H__

#include "types.h"

#define abs(x) ((x) < 0 ? -(x) : (x))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

m3 matrixMult_m3(m3 a, m3 b);
vf3 matrixMult_vf3(m3 a, v3 b);

#endif /* UTILS_H__*/