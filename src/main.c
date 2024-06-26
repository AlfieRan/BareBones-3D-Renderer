#include "types.h"
#include "utils.h"
#include "perspective.h"
#include "rendering.h"
#include "ui.h"
#include "triangulation.h"

// GLOBALS ====================================================================

static State state;
static void render();

// SETUP FUNCTIONS ========================================================

// Setup and Run the program
int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	//Debugging
	setLoggingLevel(0);

	LOG("[MAIN] Starting Program", 0);

	LOG("[MAIN] Initializing SDL", 1);
	// Initialize SDL
	ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to initialize: %s\n", SDL_GetError())

	// Create the window
	LOG("[MAIN] Creating SDL Window", 2);
	state.window = SDL_CreateWindow("W 3D Renderer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	ASSERT(state.window, "SDL failed to create window: %s\n", SDL_GetError())

	// Create the renderer
	LOG("[MAIN] Creating SDL Renderer", 2);
	state.renderer = SDL_CreateRenderer(state.window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Setup Textures
	LOG("[MAIN] Creating SDL Textures", 2); 
	state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	LOG("[MAIN] Creating SDL Debug Textures", 2); 
	state.debug = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, 128, 128);

	LOG("[MAIN] Allocating state buffers", 0);

	// Create buffers
	LOG("[MAIN] Allocating pixel buffer", 2);
	state.pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
	if (!state.pixels) {
		fprintf(stderr, "Failed to allocate memory for pixels buffer.\n");
		exit(EXIT_FAILURE);
	}

	LOG("[MAIN] Allocating bitmap", 2);
	state.bitmap = getBitMap();
	if (state.bitmap == NULL) {
		fprintf(stderr, "getBitMap() failed to provide a valid bitmap.\n");
		free(state.pixels); // Clean up previously allocated memory
		exit(EXIT_FAILURE);
	}

	LOG("[MAIN] Allocating triangle buffer", 0);
	u16 numTriangles = TRIANGLE_BUFFER_SIZE;
	Triangle* triangles = malloc(sizeof(Triangle) * numTriangles);
	usize trianglesPointer = 0;

	if (triangles == NULL) {
		fprintf(stderr, "Failed to allocate memory for triangles buffer.\n");
		free(state.pixels); // Clean up previously allocated memory
		free(state.bitmap); // Clean up previously allocated memory
		exit(EXIT_FAILURE);
	}


	LOG("[MAIN] Setting up state", 0);
	// Setup the camera
	state.camera.screen_dist = (f64)(CAMERA_SCREEN_DISTANCE);
	state.camera.position = (vf3) { 0, 0, 0 };
	state.camera.rotation = (CameraRotation) { 
		(Angle) { 0, 1, 0 },
		(Angle) { 0, 1, 0 },
		(Angle) { 0, 1, 0 }
	};
	SDL_SetRelativeMouseMode(SDL_TRUE);

	LOG("[MAIN] Starting main loop", 0);
	// Setup delta time
	u64 NOW = SDL_GetPerformanceCounter();
	u64 LAST = 0;
	f64 deltaTime = 0;

	// Main Render Loop
	while (!state.quit) {
		LOG("[MAIN] Doing initial frame setup", 2);
		SDL_Event event;
		LAST = NOW;
   		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency());
		// printf("\n[FPS] %lf", (1000 / deltaTime));

		// // Reset mouse movement
		state.mouse.change_x = 0;
		state.mouse.change_y = 0;

		// Clear the buffers
		memset(state.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
		numTriangles = TRIANGLE_BUFFER_SIZE;
		trianglesPointer = 0;
		triangles = realloc(triangles, sizeof(Triangle) * numTriangles);
		memset(triangles, 0, sizeof(Triangle) * numTriangles);

		// // Handle events
		LOG("[MAIN] Handling SDL events", 2);
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
			LOG("[MAIN] Quitting Program", 0);
			break;
		}

		const u8 *keys = SDL_GetKeyboardState(NULL);

		LOG("[MAIN] Updating Camera Rotation", 2);
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

		LOG("[MAIN] Getting Key Inputs", 2);
		// printf("\n[MOUSE CAMERA] Horizontal: (raw: %lf, cos: %lf, sin: %lf), Vertical: (raw: %lf, cos: %lf, sin: %lf)", state.camera.rotation.horizontal.raw, state.camera.rotation.horizontal.cos, state.camera.rotation.horizontal.sin, state.camera.rotation.vertical.raw, state.camera.rotation.vertical.cos, state.camera.rotation.vertical.sin);
		vf3 movementThisFrame = (vf3) { 0, 0, 0 };

		if (keys[SDLK_UP & 0xFFFF] || keys[SDL_SCANCODE_W]) {
            vf3 tmp = (vf3) {
                movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.y.sin),
                movementThisFrame.y,
				movementThisFrame.z + (MOVEMENT_SPEED * state.camera.rotation.y.cos)
            };
			movementThisFrame = tmp;
        } 
		if (keys[SDLK_DOWN & 0xFFFF] || keys[SDL_SCANCODE_S]) {
            vf3 tmp = (vf3) {
                movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.y.sin),
                movementThisFrame.y,
				movementThisFrame.z - (MOVEMENT_SPEED * state.camera.rotation.y.cos)
            };
			movementThisFrame = tmp;
        }
		if (keys[SDLK_LEFT & 0xFFFF] || keys[SDL_SCANCODE_A]) {
			vf3 tmp = (vf3) {
				movementThisFrame.x - (MOVEMENT_SPEED * state.camera.rotation.y.cos),
				movementThisFrame.y, 
				movementThisFrame.z + (MOVEMENT_SPEED * state.camera.rotation.y.sin)
			};
			movementThisFrame = tmp;
		} 
		if (keys[SDLK_RIGHT & 0xFFFF] || keys[SDL_SCANCODE_D]) {
			vf3 tmp = (vf3) {
				movementThisFrame.x + (MOVEMENT_SPEED * state.camera.rotation.y.cos),
				movementThisFrame.y,
				movementThisFrame.z - (MOVEMENT_SPEED * state.camera.rotation.y.sin)
			};
			movementThisFrame = tmp;
		}

		if (keys[SDL_SCANCODE_E]) {
			vf3 tmp = (vf3) {
				movementThisFrame.x,
				movementThisFrame.y + MOVEMENT_SPEED,
				movementThisFrame.z 
			};
			movementThisFrame = tmp;
		}

		if (keys[SDL_SCANCODE_Q]) {
			vf3 tmp = (vf3) {
				movementThisFrame.x,
				movementThisFrame.y - MOVEMENT_SPEED,
				movementThisFrame.z 
			};
			movementThisFrame = tmp;
		}

		LOG("[MAIN] Updating Camera Position", 2);
		if (movementThisFrame.x != 0 || movementThisFrame.y != 0 || movementThisFrame.z != 0) {
			state.camera.position = (vf3) {
				(f64)(state.camera.position.x + (movementThisFrame.x * deltaTime)),
				(f64)(state.camera.position.y + (movementThisFrame.y * deltaTime)),
				(f64)(state.camera.position.z + (movementThisFrame.z * deltaTime))
			};
		} else {
			LOG("[MAIN] No Camera Movement", 2);
		}


		LOG("[MAIN] Drawing Cube(s) to triangle buffer", 3);
		int size = 100;
		int h_size = size / 2;
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				u32 colour = (x + y) % 2 == 0 ? GREEN : PURPLE;
				triangles_from_cube(state, (vf3){ (x - h_size + x*10), -30, (y - h_size + y*10) }, 5, (Material){ colour, state.camera.position, 3500 }, &triangles, &trianglesPointer, &numTriangles);
			}
		}

		// triangles_from_plane(state, (vf3){ 0, -30, 70 }, 25, (Material){ PURPLE, state.camera.position, 2500 }, &triangles, &trianglesPointer, &numTriangles);

		// debug_cube_triangles(state, (vf3){ 0, -30, 70 }, 5, &triangles, &trianglesPointer, &numTriangles);
		// triangles_from_cube(state, (vf3){ 0, -30, 70 }, 5, (Material){ PURPLE, state.camera.position, 2500 }, &triangles, &trianglesPointer, &numTriangles);
		// triangles_from_cube(state, (vf3){ 10, -30, 20 }, 5, (Material){ GREEN, state.camera.position, 2500 }, &triangles, &trianglesPointer, &numTriangles);
		LOG("[MAIN] Finished drawing cubes", 3);

		// Start drawing to the screen
		// LOG("[MAIN] Getting Camera pointing", 2);
		LOG("[MAIN] Sorting Triangles", 2);
		qsort(triangles, trianglesPointer, sizeof(Triangle), compareTriangles); // sort back to front for rendering
		LOG("[MAIN] Drawing Triangles", 2);
		drawTriangles(state, triangles, trianglesPointer);

		LOG("[MAIN] Drawing UI", 2);
		drawNumber(state, 1000 / deltaTime, 4, (v2){ 10, SCREEN_HEIGHT - 10 }, 2); // Draw the FPS
		drawCrosshair(state); // Draw the crosshair last so it is on top

		LOG("[MAIN] Rendering", 2);
		render(); // Render the screen
	}

	// Cleanup and exit
	SDL_DestroyTexture(state.debug);
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
	free(state.pixels);
	// free(state.bitmap);
	free(triangles);
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
	SDL_SetRenderVSync(state.renderer, 1);
}