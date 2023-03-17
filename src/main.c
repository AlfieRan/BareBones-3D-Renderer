#include "types.h"
#include "utils.h"
#include "perspective.h"

// CONSTANTS AND MACROS =======================================================

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CROSSHAIR_SIZE 5
#define ROTATION_SPEED 0.1f
#define MOVEMENT_SPEED 0.1f

// GLOBALS ====================================================================

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture, *debug;
	u32 *pixels;
	bool quit;

	Camera camera;
} state;

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

static void drawLine(v3 a, v3 b, u32 color) {
	// Get the positions of the two points on the screen
	v2 mappedA = posToCamera(a, state.camera);
	v2 mappedB = posToCamera(b, state.camera);

	// Now use y=mx+c to get a definition for the line
	f32 m,c;
	// m = (y_b - y_a)/(x_b - x_a)
	m = (mappedB.y - mappedA.y) / (mappedB.x - mappedA.x);
	// c = y - mx
	c = mappedA.y - (m * mappedA.x);

	// Now loop through the x values, mapping them to y values
	int lo_x, hi_x, y;
	lo_x =  min(mappedA.x, mappedB.x);
	hi_x = max(mappedA.x, mappedB.x);
	for (int x = lo_x; x <= hi_x; x++) {
		y = (m * x) + c;
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}

	printf("\nLine on screen with equation: y = %lfx + %lf", m, c);

}

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

	// Center the Camera
	state.camera.position = (v3) { x: SCREEN_WIDTH / 2, y: SCREEN_HEIGHT / 2, z: 0 };

	// Main Render Loop
	while (!state.quit) {
		SDL_Event event;

		// Clear the screen
		memset(state.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					state.quit = true;
					break;
				default:
					break;
			}
		}

		if (state.quit) {
			break;
		}

		const u8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDLK_RIGHT & 0xFFFF]) {
            state.camera.rotation.horizontal.raw -= ROTATION_SPEED;
        }

        if (keys[SDLK_LEFT & 0xFFFF]) {
            state.camera.rotation.horizontal.raw += ROTATION_SPEED;
        }

		// Update Rotation State
		state.camera.rotation.horizontal.cos = cos(state.camera.rotation.horizontal.raw);
		state.camera.rotation.horizontal.sin = sin(state.camera.rotation.horizontal.raw);
		state.camera.rotation.vertical.cos = cos(state.camera.rotation.vertical.raw);
		state.camera.rotation.vertical.sin = sin(state.camera.rotation.vertical.raw);
		state.camera.rotation.matrix = rotationMatrix(state.camera.rotation.horizontal, state.camera.rotation.vertical);
		// TODO: Update the camera's display projection


		if (keys[SDLK_UP & 0xFFFF]) {
            state.camera.position = (v3) {
                state.camera.position.x + (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
                state.camera.position.y + (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				state.camera.position.z
            };
        }

        if (keys[SDLK_DOWN & 0xFFFF]) {
            state.camera.position = (v3) {
                state.camera.position.x - (MOVEMENT_SPEED * state.camera.rotation.horizontal.cos),
                state.camera.position.y - (MOVEMENT_SPEED * state.camera.rotation.horizontal.sin),
				state.camera.position.z
            };
        }

		drawLine((v3){ x:5, y:5, z:0 }, (v3){ 10, 10, 0 }, 0xFFFF00AA);

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