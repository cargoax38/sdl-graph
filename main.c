#include "headers/camera.h"
#include "headers/vec4d.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 256
#define HEIGHT 256
#define IFPS 0.0167

uint32_t framebuffer[WIDTH * HEIGHT];

void set_pixel(double x, double y, double z, uint32_t color) {
	int i = (int)floor((WIDTH - 1) * (x + 1) / 2);
	int j = (int)floor((HEIGHT - 1) * (y + 1) / 2);

	if(i < 0 || i >= WIDTH) return;
	if(j < 0 || j >= HEIGHT) return;

	int red = (color & 0xFF0000) >> 16;
	int green = (color & 0x00FF00) >> 8;
	int blue = color & 0x0000FF;
	
	uint32_t red1 = 42 + z * (red - 42);
	uint32_t green1 = 42 + z * (green - 42);
	uint32_t blue1 = 42 + z * (blue - 42);

	framebuffer[j * WIDTH + i] = ((red1 << 16) | (green1 << 8) | blue1);
}

void set_pixel_vec(struct vec4d* vec, uint32_t color) {
	if(vec == NULL) return;
	if(vec->elements[3] == 0) return;
	if(vec->elements[2] < 0.1 || vec->elements[2] > 10) return;

	double rate = 1 - (vec->elements[2] - 0.1) / 9.9;

	set_pixel(vec->elements[0] / vec->elements[3], -vec->elements[1] / vec->elements[3], rate, color);
}

// O : vec1
// A : vec2
// B : vec3
// Une autre stratégie, faire la boucle dans cette fonction mais sur le rectangle délimitant le triangle.
void draw_triangle(struct vec4d* vec1, struct vec4d* vec2, struct vec4d* vec3, uint32_t color, int i, int j) {
	if(vec1 == NULL || vec2 == NULL || vec3 == NULL) return;
	if(vec1->elements[3] == 0 || vec2->elements[3] == 0 || vec3->elements[3] == 0) return;

	double x1 = vec1->elements[0] / vec1->elements[3];
	double y1 = -vec1->elements[1] / vec1->elements[3];
	double x2 = vec2->elements[0] / vec2->elements[3];
	double y2 = -vec2->elements[1] / vec2->elements[3];
	double x3 = vec3->elements[0] / vec3->elements[3];
	double y3 = -vec3->elements[1] / vec3->elements[3];

	// Avec l'autre stratégie, on retire cette condition.
	//if((x1 > 1 || x1 < -1 || y1 > 1 || y1 < -1) && (x2 > 1 || x2 < -1 || y2 > 1 || y2 < -1) && (x3 > 1 || x3 < -1 || y3 > 1 || y3 < -1)) return;

	double x = 2 * ((double)i / 256.0) - 1;
	double y = 2 * ((double)j / 256.0) - 1;

	double b = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
	if(b == 0) return;

	double u = ((x - x1) * (y3 - y1) + (y - y1) * (x1 - x3)) / b;
	double v = ((x - x1) * (y1 - y2) + (y - y1) * (x2 - x1)) / b;

	if(u >= 0 && v >= 0 && u + v <= 1) {
		double z1 = 1 - (vec1->elements[2] - 0.1) / 9.9;
		double z2 = 1 - (vec2->elements[2] - 0.1) / 9.9;
		double z3 = 1 - (vec3->elements[2] - 0.1) / 9.9;

		double z = z1 + u * (z2 - z1) + v * (z3 - z1);

		if(z < 0 || z > 1) return;
		set_pixel(x, y, z, color);
	}
}

int main(void) {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;

	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Error while initializing SDL video");
		return EXIT_FAILURE;
	}

	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "0");

	window = SDL_CreateWindow(
		"SDL Graph view",
		2 * WIDTH,
		2 * HEIGHT,
		0
	);
	if(window == NULL) {
		fprintf(stderr, "Error while initializing SDL window");
		SDL_Quit();
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(
		window,
		NULL
	);
	if(renderer == NULL) {
		fprintf(stderr, "Error while initializing SDL renderer");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_XRGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH,
		HEIGHT
	);
	if(texture == NULL) {
		fprintf(stderr, "Error while initializing SDL texture");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	for(int i = 0; i < WIDTH * HEIGHT; i++) {
		framebuffer[i] = 0x2A2A2A;
	}

	struct camera cam;
	camera_projection(&cam, 1.22, 0.1, 10);
	camera_setpos(&cam, 0, 0, -4, 0, 0);

	struct vec4d plane[49];
	struct vec4d proj[49];

	for(int i = 0; i < 7; i++) {
		for(int j = 0; j < 7; j++) {
			plane[j + i * 7] = vec4d_init(i - 3, 0, j - 3);
		}
	}

	SDL_SetTextureScaleMode(
		texture,
		SDL_SCALEMODE_NEAREST
	);

	uint8_t running = 1;
	uint64_t begin, end;
	double elapsed;

	uint8_t keys[6] = {0, 0, 0, 0, 0, 0};

	while(running) {
		begin = SDL_GetPerformanceCounter();

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running = 0;
			}

			if(event.type == SDL_EVENT_KEY_DOWN) {
				if(event.key.key == SDLK_W) keys[0] = 1;
				if(event.key.key == SDLK_S) keys[1] = 1;
				if(event.key.key == SDLK_A) keys[2] = 1;
				if(event.key.key == SDLK_D) keys[3] = 1;
				if(event.key.key == SDLK_SPACE) keys[4] = 1;
				if(event.key.key == SDLK_LSHIFT) keys[5] = 1;

				if((event.key.mod & SDLK_LCTRL) && event.key.key == SDLK_W) {
					running = 0;
				}
			}
			if(event.type == SDL_EVENT_KEY_UP) {
				if(event.key.key == SDLK_W) keys[0] = 0;
				if(event.key.key == SDLK_S) keys[1] = 0;
				if(event.key.key == SDLK_A) keys[2] = 0;
				if(event.key.key == SDLK_D) keys[3] = 0;
				if(event.key.key == SDLK_SPACE) keys[4] = 0;
				if(event.key.key == SDLK_LSHIFT) keys[5] = 0;
			}

			if(SDL_GetWindowRelativeMouseMode(window)) {
				if(event.type == SDL_EVENT_MOUSE_MOTION) {
					camera_move(&cam, 0, 0, 0, -event.motion.xrel / 500, 0);
					camera_move(&cam, 0, 0, 0, 0, event.motion.yrel / 500);
				}

				if(event.key.key == SDLK_ESCAPE) {
					SDL_SetWindowRelativeMouseMode(window, false);
				}
			}

			if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				SDL_SetWindowRelativeMouseMode(window, true);
			}
		}

		if(keys[0]) camera_move(&cam, 0, 0, 0.1, 0, 0);
		if(keys[1]) camera_move(&cam, 0, 0, -0.1, 0, 0);
		if(keys[2]) camera_move(&cam, -0.1, 0, 0, 0, 0);
		if(keys[3]) camera_move(&cam, 0.1, 0, 0, 0, 0);
		if(keys[4]) camera_move(&cam, 0, 0.1, 0, 0, 0);
		if(keys[5]) camera_move(&cam, 0, -0.1, 0, 0, 0);

		for(int i = 0; i < WIDTH * HEIGHT; i++) {
			framebuffer[i] = 0x2A2A2A;
		}

		for(int i = 0; i < 49; i++) {
			camera_apply(&proj[i], &cam, &plane[i]);
		}

		for(int i = 0; i < WIDTH * HEIGHT; i++) {
			draw_triangle(&proj[0], &proj[1], &proj[7], 0xAA0000, i % WIDTH, (i - (i % WIDTH)) / HEIGHT);
		}

		for(int i = 0; i < 49; i++) {
			if(i == 0) set_pixel_vec(&proj[0], 0xFF0000);
			else if(i == 1) set_pixel_vec(&proj[1], 0x00FF00);
			else if(i == 7) set_pixel_vec(&proj[7], 0x0000FF);
			else set_pixel_vec(&proj[i], 0xFFFFFF);
		}

		SDL_UpdateTexture(texture, NULL, framebuffer, WIDTH * sizeof(uint32_t));

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		end = SDL_GetPerformanceCounter();

		elapsed = (double)(end - begin) / (double)SDL_GetPerformanceFrequency();
		if(elapsed < IFPS) {
			SDL_Delay(1000.0 * (IFPS - elapsed));
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
