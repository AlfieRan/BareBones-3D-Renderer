#include "types.h"

#ifndef PERSPECTIVE_H__
#define PERSPECTIVE_H__

m3 rotationMatrix(Angle horizontal, Angle vertical);
CameraPosResult posToCamera(vf3 position, Camera camera);

#endif /* PERSPECTIVE_H__*/