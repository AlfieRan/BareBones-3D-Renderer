#include "types.h"
#include "perspective.h"

Pos relativeToCamera(Pos position, Pos cameraPos) {
	position.x -= cameraPos.x;
	return position;
};