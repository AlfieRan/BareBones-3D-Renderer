#include "types.h"

#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

m3 rotationMatrix(Angle horizontal, Angle vertical);
v2 posToCamera(v3 position, Camera camera);

#endif /* PERSPECTIVE_H__*/