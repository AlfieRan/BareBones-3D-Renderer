#include "types.h"
#include "utils.h"
#include "perspective.h"

// CONSTANTS AND MACROS =======================================================

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CROSSHAIR_SIZE 5
#define ROTATION_SPEED 0.001f
#define MOVEMENT_SPEED 5
#define PI 3.14159265358979323846f

// GLOBALS ====================================================================

static State state;
static void render();

// DRAWING FUNCTIONS ==========================================================

static void verticalLine(int x, int y1, int y2, u32 color) {
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int y = y1; y <= y2; y++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

static void horizontalLine(int y, int x1, int x2, u32 color) {
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	for (int x = x1; x <= x2; x++) {
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

void drawLine(v3 a, v3 b, u32 color) {
	// Get the positions of the two points on the screen
	CameraPosResult mappedA = posToCamera(a, state.camera);
	CameraPosResult mappedB = posToCamera(b, state.camera);

	if (!mappedA.success || !mappedB.success) {
		printf("\nLine not on screen");
		return;
	}

	v2 posA = mappedA.position;
	v2 posB = mappedB.position;

	// printf("\nposA: {%d, %d} posB: {%d, %d}", posA.x, posA.y, posB.x, posB.y);

	if (posA.x != posB.x) {
		// Now use y=mx+c to get a definition for the line
		f32 m,c;
		// m = (y_b - y_a)/(x_b - x_a)
		m = (posB.y - posA.y) / (posB.x - posA.x);
		// c = y - mx
		c = posA.y - (m * posA.x);

		// Now loop through the x values, mapping them to y values
		int lo_x, hi_x, y;
		lo_x =  min(posA.x, posB.x);
		hi_x = max(posA.x, posB.x);
		for (int x = lo_x; x <= hi_x; x++) {
			y = (m * x) + c;
			state.pixels[y * SCREEN_WIDTH + x] = color;
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

void drawEmptyCube(v3 a, v3 b, v3 c, v3 d, v3 e, v3 f, v3 g, v3 h, u32 color) {
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

void drawEmptySquare(v3 a, v3 b, v3 c, v3 d, u32 color) {
	drawLine(a, b, color);
	drawLine(a, c, color);
	drawLine(b, d, color);
	drawLine(c, d, color);
}

void drawTestSquare(v3 center, u32 length) {
	u32 h = length / 2;
	drawEmptySquare(
		(v3) { center.x - h, center.y, center.z + h },
		(v3) { center.x + h, center.y, center.z + h },
		(v3) { center.x - h, center.y, center.z - h },
		(v3) { center.x + h, center.y, center.z - h },
		0xFFFF00AA
	);
}

void drawTestCube(v3 center, u32 length) {
	u32 h = length / 2;
	drawEmptyCube(
		(v3) { center.x - h, center.y - h, center.z + h },
		(v3) { center.x - h, center.y + h, center.z + h },
		(v3) { center.x - h, center.y - h, center.z - h },
		(v3) { center.x - h, center.y + h, center.z - h },
		(v3) { center.x + h, center.y - h, center.z + h },
		(v3) { center.x + h, center.y + h, center.z + h },
		(v3) { center.x + h, center.y - h, center.z - h },
		(v3) { center.x + h, center.y + h, center.z - h },
		0xFFFF00AA
	);
}

// static void drawPlane(Plane plane) {}

static void drawCrosshair() {
	// Draw the crosshair
	int centerX, centerY;
	centerX = SCREEN_WIDTH / 2;
	centerY = SCREEN_HEIGHT / 2;

	verticalLine(centerX, centerY - CROSSHAIR_SIZE, centerY + CROSSHAIR_SIZE, 0xFFFFFFFF);
	horizontalLine(centerY, centerX - CROSSHAIR_SIZE, centerX + CROSSHAIR_SIZE, 0xFFFFFFFF);
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
	state.camera.position = (v3) { x: SCREEN_WIDTH / 2, y: 0, z: SCREEN_HEIGHT / 2 };
	state.camera.fov = (FOV) { horizontal: 1, vertical: 1 };
	state.camera.screen = (SCREEN) { horizontal: SCREEN_WIDTH, vertical: SCREEN_HEIGHT };
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Main Render Loop
	while (!state.quit) {
		SDL_Event event;

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

		// Update Rotation State
		state.camera.rotation.horizontal.raw += state.mouse.change_x * ROTATION_SPEED;
		state.camera.rotation.vertical.raw += state.mouse.change_y * ROTATION_SPEED;
		if (state.camera.rotation.horizontal.raw > 2 * PI) {
			state.camera.rotation.horizontal.raw -= 2 * PI;
		} else if (state.camera.rotation.horizontal.raw < 0) {
			state.camera.rotation.horizontal.raw += 2 * PI;
		}
		if (state.camera.rotation.vertical.raw > 2 * PI) {
			state.camera.rotation.vertical.raw -= 2 * PI;
		} else if (state.camera.rotation.vertical.raw < 0) {
			state.camera.rotation.vertical.raw += 2 * PI;
		}

		state.camera.rotation.horizontal.cos = cos(state.camera.rotation.horizontal.raw);
		state.camera.rotation.horizontal.sin = sin(state.camera.rotation.horizontal.raw);
		state.camera.rotation.vertical.cos = cos(state.camera.rotation.vertical.raw);
		state.camera.rotation.vertical.sin = sin(state.camera.rotation.vertical.raw);
		state.camera.rotation.matrix = rotationMatrix(state.camera.rotation.horizontal, state.camera.rotation.vertical);

		// printf("\n[MOUSE CAMERA] Horizontal: (raw: %lf, cos: %lf, sin: %lf), Vertical: (raw: %lf, cos: %lf, sin: %lf)", state.camera.rotation.horizontal.raw, state.camera.rotation.horizontal.cos, state.camera.rotation.horizontal.sin, state.camera.rotation.vertical.raw, state.camera.rotation.vertical.cos, state.camera.rotation.vertical.sin);
		v3 movementThisFrame = (v3) { 0, 0, 0 };

		if (keys[SDLK_UP & 0xFFFF]) {
            v3 tmp = (v3) {
                movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
                movementThisFrame.y + (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				movementThisFrame.z
            };
			movementThisFrame = tmp;
        } 
		if (keys[SDLK_DOWN & 0xFFFF]) {
            v3 tmp = (v3) {
                movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
                movementThisFrame.y - (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				movementThisFrame.z
            };
			movementThisFrame = tmp;
        }
		if (keys[SDLK_LEFT & 0xFFFF]) {
			v3 tmp = (v3) {
				movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				movementThisFrame.y + (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
				movementThisFrame.z
			};
			movementThisFrame = tmp;
		} 
		if (keys[SDLK_RIGHT & 0xFFFF]) {
			v3 tmp = (v3) {
				movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				movementThisFrame.y - (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
				movementThisFrame.z
			};
			movementThisFrame = tmp;
		}

		if (movementThisFrame.x != 0 || movementThisFrame.y != 0 || movementThisFrame.z != 0) {
			state.camera.position = (v3) {
				state.camera.position.x + movementThisFrame.x,
				state.camera.position.y + movementThisFrame.y,
				state.camera.position.z + movementThisFrame.z
			};
		}

		printf("\n[MOUSE CAMERA] Position: (%i, %i, %i)", state.camera.position.x, state.camera.position.y, state.camera.position.z);

		// drawLine((v3){ 0, 10, SCREEN_HEIGHT / 2 }, (v3){ 100, 10, SCREEN_HEIGHT / 2 }, 0xFFFF00AA);
		drawTestCube((v3){ SCREEN_WIDTH / 2, 100, SCREEN_HEIGHT / 2 }, 10);

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