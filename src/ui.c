#include "ui.h"

void drawCrosshair(State state) {
	// Draw the crosshair
	int centerX, centerY;
	centerX = SCREEN_WIDTH / 2;
	centerY = SCREEN_HEIGHT / 2;

	verticalLine(state, centerX, centerY - CROSSHAIR_SIZE, centerY + CROSSHAIR_SIZE, 0xFFFFFFFF);
	horizontalLine(state, centerY, centerX - CROSSHAIR_SIZE, centerX + CROSSHAIR_SIZE, 0xFFFFFFFF);
}

void drawSingleNumber(State state, u8 number, v2 position, u8 scale) {
	if (number > 9) return;
	char* bitMap = getBitMapItem(state, number);
	if (bitMap == NULL) return;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 6; x++) {
			if (bitMap[(y * 6) + x] == '1') {
				for (int i = 0; i < scale; i++) {
					for (int j = 0; j < scale; j++) {
						int pixelX = position.x + x * scale + i;
						int pixelY = position.y - y * scale + j;
						if (pixelX >= 0 && pixelX < SCREEN_WIDTH && pixelY >= 0 && pixelY < SCREEN_HEIGHT) {
							state.pixels[pixelY * SCREEN_WIDTH + pixelX] = 0xFFFFFFFF;
						}
					}
				}
			}
		}
	}
	free(bitMap);
}

void drawNumber(State state, u8 number, u8 digits, v2 position, u8 scale) {
	u8 digitArray[digits];

	for (int i = 0; i < digits; i++) {
		digitArray[digits - i - 1] = number % 10;
		number /= 10;
	}

	int offsetMult = 6 * scale;
	for (int i = 0; i < digits; i++) {
		drawSingleNumber(state, digitArray[i], (v2) { position.x + (i * offsetMult), position.y }, scale);
	}
}
