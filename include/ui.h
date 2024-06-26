#ifndef UI_H__
#define UI_H__

#include "types.h"
#include "perspective.h"
#include "rendering.h"
#include "utils.h"

void drawCrosshair(State state);
void drawSingleNumber(State state, u8 number, v2 position, u8 scale);
void drawNumber(State state, u8 number, u8 digits, v2 position, u8 scale);

#endif /* UI_H__*/