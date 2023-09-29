#include "types.h"
#include "utils.h"
#include "perspective.h"
#include "rendering.h"
#include "ui.h"

// GLOBALS ====================================================================

static State state;
static void render();

// SETUP FUNCTIONS ========================================================

// Setup and Run the program
int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	printf("\n[MAIN] Initializing SDL");
	// Initialize SDL
	ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to initialize: %s\n", SDL_GetError())

	// Create the window
	state.window = SDL_CreateWindow("W 3D Renderer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	ASSERT(state.window, "SDL failed to create window: %s\n", SDL_GetError())

	// Create the renderer
	state.renderer = SDL_CreateRenderer(state.window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Setup Textures
	state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	state.debug = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, 128, 128);

	printf("\n[MAIN] Allocating pixel buffer");
	// Create the pixel buffer
	state.pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));

	printf("\n[MAIN] Setting up state");
	// Setup the camera
	state.camera.screen_dist = 1;
	state.camera.position = (vf3) { 0, 0, 0 };
	state.camera.rotation = (CameraRotation) { 
		(Angle) { 0, 1, 0 },
		(Angle) { 0, 1, 0 },
		(Angle) { 0, 1, 0 }
	};
	SDL_SetRelativeMouseMode(SDL_TRUE);

	printf("\n[MAIN] Starting main loop");
	// Setup delta time
	u64 NOW = SDL_GetPerformanceCounter();
	u64 LAST = 0;
	f64 deltaTime = 0;

	// Main Render Loop
	while (!state.quit) {
		printf("\n[MAIN] Doing initial frame setup");
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

		// Handle events
		printf("\n[MAIN] Handling SDL events");
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
			printf("\n[MAIN] Quitting Program");
			break;
		}

		const u8 *keys = SDL_GetKeyboardState(NULL);

		printf("\n[MAIN] Updating Camera Rotation");
		// Update Camera Rotation
		state.camera.rotation.x.raw += state.mouse.change_y * ROTATION_SPEED;
		state.camera.rotation.y.raw += state.mouse.change_x * ROTATION_SPEED;

		// Clamp the rotation
		if (state.camera.rotation.x.raw > VERTICAL_FOV) {
			state.camera.rotation.x.raw = VERTICAL_FOV;
		} else if (state.camera.rotation.x.raw < -VERTICAL_FOV) {
			state.camera.rotation.x.raw = -VERTICAL_FOV;
		}
		if (state.camera.rotation.y.raw > TWO_PI) {
			state.camera.rotation.y.raw -= TWO_PI;
		} else if (state.camera.rotation.y.raw < 0) {
			state.camera.rotation.y.raw += TWO_PI;
		}
		
		// printf("\n\n[CAMERA] Rotation (%lf, %lf, %lf) \n", state.camera.rotation.x.raw, state.camera.rotation.y.raw, state.camera.rotation.z.raw);

		// Update the sin and cos values
		state.camera.rotation.x.sin = sin(state.camera.rotation.x.raw);
		state.camera.rotation.x.cos = cos(state.camera.rotation.x.raw);
		state.camera.rotation.y.sin = sin(state.camera.rotation.y.raw);
		state.camera.rotation.y.cos = cos(state.camera.rotation.y.raw);
		state.camera.rotation.z.sin = sin(state.camera.rotation.z.raw);
		state.camera.rotation.z.cos = cos(state.camera.rotation.z.raw);

		printf("\n[MAIN] Getting Key Inputs");
		// printf("\n[MOUSE CAMERA] Horizontal: (raw: %lf, cos: %lf, sin: %lf), Vertical: (raw: %lf, cos: %lf, sin: %lf)", state.camera.rotation.horizontal.raw, state.camera.rotation.horizontal.cos, state.camera.rotation.horizontal.sin, state.camera.rotation.vertical.raw, state.camera.rotation.vertical.cos, state.camera.rotation.vertical.sin);
		v3 movementThisFrame = (v3) { 0, 0, 0 };

		if (keys[SDLK_UP & 0xFFFF] || keys[SDL_SCANCODE_W]) {
            v3 tmp = (v3) {
                movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.y.sin),
                movementThisFrame.y,
				movementThisFrame.z + (MOVEMENT_SPEED * state.camera.rotation.y.cos)
            };
			movementThisFrame = tmp;
        } 
		if (keys[SDLK_DOWN & 0xFFFF] || keys[SDL_SCANCODE_S]) {
            v3 tmp = (v3) {
                movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.y.sin),
                movementThisFrame.y,
				movementThisFrame.z - (MOVEMENT_SPEED * state.camera.rotation.y.cos)
            };
			movementThisFrame = tmp;
        }
		if (keys[SDLK_LEFT & 0xFFFF] || keys[SDL_SCANCODE_A]) {
			v3 tmp = (v3) {
				movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.y.cos),
				movementThisFrame.y, 
				movementThisFrame.z + (MOVEMENT_SPEED * state.camera.rotation.y.sin)
			};
			movementThisFrame = tmp;
		} 
		if (keys[SDLK_RIGHT & 0xFFFF] || keys[SDL_SCANCODE_D]) {
			v3 tmp = (v3) {
				movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.y.cos),
				movementThisFrame.y,
				movementThisFrame.z - (MOVEMENT_SPEED * state.camera.rotation.y.sin)
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

		printf("\n[MAIN] Updating Camera Position");
		if (movementThisFrame.x != 0 || movementThisFrame.y != 0 || movementThisFrame.z != 0) {
			state.camera.position = (vf3) {
				state.camera.position.x + (movementThisFrame.x / deltaTime),
				state.camera.position.y + (movementThisFrame.y / deltaTime),
				state.camera.position.z + (movementThisFrame.z / deltaTime)
			};
		}

		printf("\n[MAIN] Drawing Cubes");
		drawTestCube(state, (vf3){ 0, 0, 100 }, 100, PURPLE);
		drawTestCube(state, (vf3){ 100, 200, 100 }, 100, GREEN);
		drawTestCube(state, (vf3){ 100, 300, 100 }, 100, PURPLE);
		drawTestCube(state, (vf3){ 100, 400, 100 }, 100, GREEN);
		drawTestCube(state, (vf3){ 100, 200, 100 }, 100, PURPLE);
		drawTestCube(state, (vf3){ 100, 300, 100 }, 100, GREEN);

		// Draw a cube with a green outline
		// drawSquare(state, (vf3){ 0, 0, 100 }, 3, RED);
		drawCube(state, (vf3){ 100, 0, 100 }, 100, PURPLE);
		drawTestCube(state, (vf3){ 100, 0, 100 }, 100, GREEN);

		// Debug 3d compass
		// drawLine((vf3){ 0, 0, 0 }, (vf3){ 0, 0, 10 }, BLUE);
		// drawLine((vf3){ 0, 0, 5 }, (vf3){ 0, 10, 5 }, GREEN);
		// drawLine((vf3){ 0, 0, 0 }, (vf3){ 10, 0, 0 }, RED);

		printf("\n[MAIN] Drawing UI");
		drawNumber(state, 1000 / deltaTime, 4, (v2){ 10, SCREEN_HEIGHT - 10 }); // Draw the FPS
		drawCrosshair(state); // Draw the crosshair last so it is on top
		printf("\n[MAIN] Rendering");
		render(); // Render the screen
		// state.quit = true;
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