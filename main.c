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

void set_pixel(double x, double y, uint32_t color) {
	int i = (int)floor((WIDTH - 1) * (x + 1) / 2);
	int j = (int)floor((HEIGHT - 1) * (y + 1) / 2);

	if(i < 0 || i >= WIDTH) return;
	if(j < 0 || j >= HEIGHT) return;

	framebuffer[j * WIDTH + i] = color;
}

void set_pixel_vec(struct vec4d* vec, uint32_t color) {
	if(vec == NULL) return;
	if(vec->elements[3] == 0) return;
	if(vec->elements[2] < 0.1 || vec->elements[2] > 10) return;

	set_pixel(vec->elements[0] / vec->elements[3], -vec->elements[1] / vec->elements[3], color);
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
		SDL_PIXELFORMAT_XBGR8888,
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
	camera_setpos(&cam, 0, 1, -4, 0, 0);

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

	while(running) {
		begin = SDL_GetPerformanceCounter();

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
			if(SDL_GetWindowRelativeMouseMode(window)) {
				if(event.type == SDL_EVENT_MOUSE_MOTION) {
					camera_move(&cam, 0, 0, 0, -event.motion.xrel / 1000, 0);
					camera_move(&cam, 0, 0, 0, 0, event.motion.yrel / 1000);
				}

				if(event.key.key == SDLK_ESCAPE) {				
					SDL_SetWindowRelativeMouseMode(window, false);
				}

				if(event.key.key == SDLK_W) {
					camera_move(&cam, 0, 0, 0.1, 0, 0);
				}
				if(event.key.key == SDLK_S) {
					camera_move(&cam, 0, 0, -0.1, 0, 0);
				}
				if(event.key.key == SDLK_A) {
					camera_move(&cam, -0.1, 0, 0, 0, 0);
				}
				if(event.key.key == SDLK_D) {
					camera_move(&cam, 0.1, 0, 0, 0, 0);
				}
				if(event.key.key == SDLK_SPACE) {
					camera_move(&cam, 0, 0.1, 0, 0, 0);
				}
				if(event.key.key == SDLK_LSHIFT) {
					camera_move(&cam, 0, -0.1, 0, 0, 0);
				}
			}

			if(event.type == SDL_EVENT_MOUSE_MOTION) {
				if(SDL_GetWindowRelativeMouseMode(window)) {
					camera_move(&cam, 0, 0, 0, -event.motion.xrel / 200, 0);
					camera_move(&cam, 0, 0, 0, 0, event.motion.yrel / 200);
				}
			}
			if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				SDL_SetWindowRelativeMouseMode(window, true);
			}

			if(event.type == SDL_EVENT_KEY_DOWN) {
				if((event.key.mod & SDLK_LCTRL) && event.key.key == SDLK_W) {
					running = 0;
				}
			}
		}

		for(int i = 0; i < WIDTH * HEIGHT; i++) {
			framebuffer[i] = 0x2A2A2A;
		}

		for(int i = 0; i < 49; i++) {
			camera_apply(&proj[i], &cam, &plane[i]);
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
