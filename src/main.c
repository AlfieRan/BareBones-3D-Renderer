#include "types.h"
#include "utils.h"
#include "perspective.h"



// GLOBALS ====================================================================

static State state;
static void render();

// DRAWING FUNCTIONS ==========================================================

void verticalLine(int x, int y1, int y2, u32 color) {
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int y = y1; y <= y2; y++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

void horizontalLine(int y, int x1, int x2, u32 color) {
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	for (int x = x1; x <= x2; x++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

void drawLine(vf3 a, vf3 b, u32 color) {
	// Get the positions of the two points on the screen
	v2 posA = point_to_screen(state.camera.rotation, state.camera.position, state.camera.screen_dist, a);
	v2 posB = point_to_screen(state.camera.rotation, state.camera.position, state.camera.screen_dist, b);

	printf("\nposA: {%d, %d} posB: {%d, %d}", posA.x, posA.y, posB.x, posB.y);

	if (posA.x != posB.x) {
		// Now use y=mx+c to get a definition for the line
		// m = (y_b - y_a)/(x_b - x_a)
		f32 m = (f32)((f32)(posB.y - posA.y) / (f32)(posB.x - posA.x));
		// c = y - mx
		f32 c = (f32)(posA.y - (f32)(m * posA.x));
	
		// Now loop through the x values, mapping them to y values
		int lo_x, hi_x, raw_lo_x, raw_hi_x, y;
		raw_lo_x = min(posA.x, posB.x);
		raw_hi_x = max(posA.x, posB.x);
		lo_x =  max(raw_lo_x, 0);
		hi_x = min(raw_hi_x, SCREEN_WIDTH);
		for (int x = lo_x; x <= hi_x; x++) {
			y = (m * x) + c;
			if (y > 0 && y < SCREEN_HEIGHT) {
				state.pixels[y * SCREEN_WIDTH + x] = color;
			}
		}
	} else {
		// If the x values are the same, just draw a vertical line
		int lo_y, hi_y;
		lo_y = min(posA.y, posB.y);
		hi_y = max(posA.y, posB.y);
		for (int y = lo_y; y <= hi_y; y++) {
			state.pixels[y * SCREEN_WIDTH + posA.x] = color;
		}
	}

}

void drawEmptyCube(vf3 a, vf3 b, vf3 c, vf3 d, vf3 e, vf3 f, vf3 g, vf3 h, u32 color) {
	drawLine(a, b, color);
	drawLine(a, c, color);
	drawLine(a, e, color);
	drawLine(b, f, color);
	drawLine(b, d, color);
	drawLine(c, g, color);
	drawLine(c, d, color);
	drawLine(d, h, color);
	drawLine(e, f, color);
	drawLine(e, g, color);
	drawLine(f, h, color);
	drawLine(g, h, color);
}

void drawEmptySquare(vf3 a, vf3 b, vf3 c, vf3 d, u32 color) {
	drawLine(a, b, color);
	drawLine(a, c, color);
	drawLine(b, d, color);
	drawLine(c, d, color);
}

void drawTestSquare(vf3 center, u32 length) {
	u32 h = length / 2;
	drawEmptySquare(
		(vf3) { center.x - h, center.y, center.z + h },
		(vf3) { center.x + h, center.y, center.z + h },
		(vf3) { center.x - h, center.y, center.z - h },
		(vf3) { center.x + h, center.y, center.z - h },
		0xFFFF00AA
	);
}

void drawTestCube(vf3 center, u32 length, u32 color) {
	u32 h = length / 2;
	drawEmptyCube(
		(vf3) { center.x - h, center.y - h, center.z + h },
		(vf3) { center.x - h, center.y + h, center.z + h },
		(vf3) { center.x - h, center.y - h, center.z - h },
		(vf3) { center.x - h, center.y + h, center.z - h },
		(vf3) { center.x + h, center.y - h, center.z + h },
		(vf3) { center.x + h, center.y + h, center.z + h },
		(vf3) { center.x + h, center.y - h, center.z - h },
		(vf3) { center.x + h, center.y + h, center.z - h },
		color
	);
} 

static void drawCrosshair() {
	// Draw the crosshair
	int centerX, centerY;
	centerX = SCREEN_WIDTH / 2;
	centerY = SCREEN_HEIGHT / 2;

	verticalLine(centerX, centerY - CROSSHAIR_SIZE, centerY + CROSSHAIR_SIZE, 0xFFFFFFFF);
	horizontalLine(centerY, centerX - CROSSHAIR_SIZE, centerX + CROSSHAIR_SIZE, 0xFFFFFFFF);
}

static void drawSingleNumber(u8 number, v2 position) {
	if (number > 9) return;
	char* bitMap = getNumberBitMap(number);
	if (bitMap == NULL) return;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 6; x++) {
			if (bitMap[(y * 6) + x] == '1') {
				state.pixels[(position.y - y) * SCREEN_WIDTH + (position.x + x)] = 0xFFFFFFFF;
			}
		}
	}
	free(bitMap);
}

static void drawNumber(u8 number, u8 digits, v2 position) {
	u8 digitArray[digits];

	for (int i = 0; i < digits; i++) {
		digitArray[digits - i - 1] = number % 10;
		number /= 10;
	}

	for (int i = 0; i < digits; i++) {
		drawSingleNumber(digitArray[i], (v2) { position.x + (i * 6), position.y });
	}
}

// SETUP FUNCTIONS ========================================================

// Setup and Run the program
int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// Initialize SDL
	ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to initialize: %s\n", SDL_GetError())

	// Create the window
	state.window = SDL_CreateWindow("W Renderer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	ASSERT(state.window, "SDL failed to create window: %s\n", SDL_GetError())

	// Create the renderer
	state.renderer = SDL_CreateRenderer(state.window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Setup Textures
	state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	state.debug = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, 128, 128);

	// Create the pixel buffer
	state.pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));

	// Setup the camera
	state.camera.screen_dist = 1;
	state.camera.position = (vf3) { 0, 0, 0 };
	state.camera.rotation = (vf3) { 0, 0, 0 };
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Setup delta time
	u64 NOW = SDL_GetPerformanceCounter();
	u64 LAST = 0;
	f64 deltaTime = 0;

	// Main Render Loop
	while (!state.quit) {
		SDL_Event event;
		LAST = NOW;
   		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );
		// printf("\n[FPS] %lf", (1000 / deltaTime));

		// Reset mouse movement
		state.mouse.change_x = 0;
		state.mouse.change_y = 0;

		// Clear the screen
		memset(state.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					state.quit = true;
					break;
				case SDL_EVENT_MOUSE_MOTION:
					state.mouse.change_x = event.motion.xrel;
					state.mouse.change_y = event.motion.yrel;
					break;
				default:
					break;
			}
		}

		if (state.quit) {
			break;
		}

		const u8 *keys = SDL_GetKeyboardState(NULL);

		// TODO - update camera rotation

		// printf("\n[MOUSE CAMERA] Horizontal: (raw: %lf, cos: %lf, sin: %lf), Vertical: (raw: %lf, cos: %lf, sin: %lf)", state.camera.rotation.horizontal.raw, state.camera.rotation.horizontal.cos, state.camera.rotation.horizontal.sin, state.camera.rotation.vertical.raw, state.camera.rotation.vertical.cos, state.camera.rotation.vertical.sin);
		v3 movementThisFrame = (v3) { 0, 0, 0 };

		if (keys[SDLK_UP & 0xFFFF] || keys[SDL_SCANCODE_W]) {
            v3 tmp = (v3) {
                movementThisFrame.x + MOVEMENT_SPEED,
                movementThisFrame.y + MOVEMENT_SPEED,
				movementThisFrame.z
            };
			movementThisFrame = tmp;
        } 
		if (keys[SDLK_DOWN & 0xFFFF] || keys[SDL_SCANCODE_S]) {
            v3 tmp = (v3) {
                movementThisFrame.x - MOVEMENT_SPEED,
                movementThisFrame.y,
				movementThisFrame.z - MOVEMENT_SPEED,
            };
			movementThisFrame = tmp;
        }
		if (keys[SDLK_LEFT & 0xFFFF] || keys[SDL_SCANCODE_A]) {
			v3 tmp = (v3) {
				movementThisFrame.x - MOVEMENT_SPEED,
				movementThisFrame.y, 
				movementThisFrame.z + MOVEMENT_SPEED,
			};
			movementThisFrame = tmp;
		} 
		if (keys[SDLK_RIGHT & 0xFFFF] || keys[SDL_SCANCODE_D]) {
			v3 tmp = (v3) {
				movementThisFrame.x + MOVEMENT_SPEED,
				movementThisFrame.y,
				movementThisFrame.z - MOVEMENT_SPEED,
			};
			movementThisFrame = tmp;
		}

		if (keys[SDL_SCANCODE_E]) {
			v3 tmp = (v3) {
				movementThisFrame.x,
				movementThisFrame.y + MOVEMENT_SPEED,
				movementThisFrame.z 
			};
			movementThisFrame = tmp;
		}

		if (keys[SDL_SCANCODE_Q]) {
			v3 tmp = (v3) {
				movementThisFrame.x,
				movementThisFrame.y - MOVEMENT_SPEED,
				movementThisFrame.z 
			};
			movementThisFrame = tmp;
		}

		if (movementThisFrame.x != 0 || movementThisFrame.y != 0 || movementThisFrame.z != 0) {
			state.camera.position = (vf3) {
				state.camera.position.x + (movementThisFrame.x * deltaTime),
				state.camera.position.y + (movementThisFrame.y * deltaTime),
				state.camera.position.z + (movementThisFrame.z * deltaTime)
			};
		}

		drawTestCube((vf3){ 0, 0, 100 }, 100, PURPLE);
		// drawTestCube((vf3){ 100, 200, 100 }, 100, GREEN);
		// drawTestCube((vf3){ 100, 300, 100 }, 100, PURPLE);
		// drawTestCube((vf3){ 100, 400, 100 }, 100, GREEN);
		// drawTestCube((vf3){ 100, 200, 100 }, 100, PURPLE);
		drawTestCube((vf3){ 100, 300, 100 }, 100, GREEN);
		// drawLine((vf3){ 0, 0, 0 }, (vf3){ 0, 0, 100 }, PURPLE);
		// drawLine((vf3){ 0, 0, 0 }, (vf3){ 0, 100, 0 }, GREEN);
		// drawLine((vf3){ 0, 0, 0 }, (vf3){ 100, 0, 0 }, RED);


		drawNumber(1000 / deltaTime, 4, (v2){ 10, SCREEN_HEIGHT - 10 }); // Draw the FPS
		drawCrosshair(); // Draw the crosshair last so it is on top
		render(); // Render the screen
	}

	// Cleanup and exit
	SDL_DestroyTexture(state.debug);
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
} 


// RENDERING FUNCTIONS ========================================================

// Render what is drawn in the pixel buffer to the screen
static void render() {
	void *px;
    int pitch;
    SDL_LockTexture(state.texture, NULL, &px, &pitch);
    {
        for (usize y = 0; y < SCREEN_HEIGHT; y++) {
            memcpy(
                &((u8*) px)[y * pitch],
                &state.pixels[y * SCREEN_WIDTH],
                SCREEN_WIDTH * 4);
        }
    }
    SDL_UnlockTexture(state.texture);	

	SDL_SetRenderTarget(state.renderer, NULL);
    SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 0xFF);
    SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);

	SDL_RenderClear(state.renderer);
    SDL_RenderTextureRotated(state.renderer, state.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);

    SDL_SetTextureBlendMode(state.debug, SDL_BLENDMODE_BLEND);
    SDL_RenderTexture(state.renderer, state.debug, NULL, &((SDL_FRect) { 0, 0, 512, 512 }));
    SDL_RenderPresent(state.renderer);
}