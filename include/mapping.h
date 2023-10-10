#ifndef MAPPING_H__
#define MAPPING_H__
#include "types.h"

#define LINE_SIZE 100 // Assume no individual line is larger than 100 characters - this might need to be increased in the future.
#define MODE_SWITCH_CHAR "["
#define MODE_SWITCH_CHAR_END "]"
#define DATA_START_CHAR "("
#define DATA_END_CHAR ")"
#define DATA_SEPERATOR_CHAR ","
#define MODE_TRIANGLES 2
#define MODE_MATERIALS 1

typedef struct { Material* materials; Triangle* triangles; } MAP;

#endif /* MAPPING_H__*/