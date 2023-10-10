#include "mapping.h"


// Modes are as follows:
//	0. Do nothing.
//	1. Record Material Data.
// 	2. Record Triangle Data.


MAP loadMap(char mapName) {
	FILE *file = fopen(("./assets/maps/" + mapName), "r");

	if (file == NULL) {
        LOG("Error: could not open the num_chars.txt file", 0);
        exit(1);
    };

	u16 numMaterials = 10;
	u16 pointerMaterials = 0;
	u16 numTriangles = 100;
	u16 pointerTriangles = 0;

	MAP map;
	map.materials = malloc(numMaterials * sizeof(Material));
	map.triangles = malloc(numTriangles * sizeof(Triangle));

	
	char line[LINE_SIZE]; 
	bool finished = false;
	u16 pointer = 0;
	u16 mode = 0;
	
	while (!finished) {
		if (fgets(line, LINE_SIZE, file) == NULL) {
			// Check if we've reached the end of the file.
			finished = true;
		}

		line[strcspn(line, "\n")] = '\0';

		if (line[0] == "[") {
			// mode switch
			mode = readModeSwitch(line, mode);

		} else if (line[0] == "(") {
			// data switch
			if (mode == 1) {
				if (pointerTriangles + 2 > numTriangles) {
					numTriangles += 100;
					realloc(map.triangles, numTriangles * sizeof(Triangle));
				}

				Triangle* triangle = readTriangle(line, map.materials);

				if (triangle != NULL) {
					map.triangles[pointerTriangles] = &triangle;
					pointerTriangles += 1;
				}

				
			} else if (mode == 2) {
				if (pointerMaterials + 2 > numMaterials) {
					numMaterials += 100;
					realloc(map.materials, numMaterials * sizeof(Material));
				}

				Material* material = readMaterial(line);

				if (material != NULL) {
					memcpy((*map.materials)[pointerMaterials], &material, sizeof(Material));
					pointerMaterials += 1;
				}
			}

		}	

		// increase pointer ready for next line.
		pointer += 1;
	}

	fclose(file);
}

u16 readModeSwitch(char* line, u16 mode) {
	char* token;

	token = strtok_r(line, MODE_SWITCH_CHAR, &line);
	token = strtok_r(token, MODE_SWITCH_CHAR_END, &token);

	if (token == MODE_MATERIALS) {
		return 1;
	} else if (token == MODE_TRIANGLES) {
		return 2;
	} else {
		printf("[MAP LOADER] Could not read mode switch with token: %c", &token);
		return mode;
	}
}

Material* readMaterial(char* line) {
	char* remainder_of_line = &line;
	char* token;

	for (u8 i = 0; i < 3; i++) {
		token = strtok_r(remainder_of_line, DATA_SEPERATOR_CHAR, &remainder_of_line);
	}

	return (Material){ 0, (vf3){ 0, 0, 0}, 0 };
}

Triangle* readTriangle(char* line, Material* materials) {
	return (Triangle){ (vf3){0,0,0}, (vf3){0,0,0}, (vf3){0,0,0}, materials[0], 0, true };
}