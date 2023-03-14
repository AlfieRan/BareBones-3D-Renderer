#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1);}

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
typedef ssize_t  isize;
typedef struct { u32 x,y,z; } Pos;
typedef struct { Pos center; u8 length; u32 color; } Cube;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture, *debug;
	u32 *pixels;
	bool quit;

	struct {
		Pos pos;
	} camera;
} state;

static void render();

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

static void drawCube(Cube cube) {
	// For now, ignore 3d
	u32 lo_x, hi_x, lo_y, hi_y;
	lo_x = cube.center.x - cube.length;
	hi_x = cube.center.x + cube.length;
	lo_y = cube.center.y - cube.length;
	hi_y = cube.center.y + cube.length;

	while (lo_x < hi_x) {
		verticalLine(lo_x, lo_y, hi_y, cube.color);
		lo_x++;
	}
}

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
	state.camera.pos = (Pos) { x: SCREEN_WIDTH / 2, y: SCREEN_HEIGHT / 2, z: 0 };

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
			state.camera.pos.x += 1;
		}

		if (keys[SDLK_LEFT & 0xFFFF]) {
			state.camera.pos.x -= 1;
		}

		if (keys[SDLK_UP & 0xFFFF]) {
			state.camera.pos.y += 1;
		}

		if (keys[SDLK_DOWN & 0xFFFF]) {
			state.camera.pos.y -= 1;
		}

		drawCube(((Cube) { center:  state.camera.pos, length: 10, color: 0xFF00FF00 }));
		render();
	}

	// Cleanup and exit
	SDL_DestroyTexture(state.debug);
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
} 


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