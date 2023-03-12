#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	u32 *pixels;
	bool quit;
};

int main(int argc, char *argv[]) {
	ASSERT(
		!SDL_Init(SDL_INIT_VIDEO),
		"SDL failed to initialize: %s\n",
		SDL_GetError()
	)
}