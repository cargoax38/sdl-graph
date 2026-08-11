#include "headers/camera.h"
#include "headers/vec4d.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 128
#define HEIGHT 128
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
	if(vec->elements[2] < 1 || vec->elements[2] > 10) return;

	set_pixel(vec->elements[0] / vec->elements[3], vec->elements[1] / vec->elements[3], color);
}

int main(void) {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;

	//printf("| %.2f |\n| %.2f |\n| %.2f |\n| %.2f |\n", pos1.elements[0], pos1.elements[1], pos1.elements[2], pos1.elements[3]);

	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Error while initializing SDL video");
		return EXIT_FAILURE;
	}

	window = SDL_CreateWindow(
		"SDL Graph view",
		4 * WIDTH,
		4 * HEIGHT,
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
	camera_projection(&cam, 1.22, 1, 10);
	camera_setpos(&cam, 0, 0, -4, 0, 0);

	struct vec4d pos1 = vec4d_init(-1, -1, -1);
	struct vec4d pos2 = vec4d_init(-1, -1, 1);
	struct vec4d pos3 = vec4d_init(-1, 1, -1);
	struct vec4d pos4 = vec4d_init(-1, 1, 1);
	struct vec4d pos5 = vec4d_init(1, -1, -1);
	struct vec4d pos6 = vec4d_init(1, -1, 1);
	struct vec4d pos7 = vec4d_init(1, 1, -1);
	struct vec4d pos8 = vec4d_init(1, 1, 1);

	struct vec4d pro1, pro2, pro3, pro4, pro5, pro6, pro7, pro8;

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
			if(event.type == SDL_EVENT_KEY_DOWN) {
				if((event.key.mod & SDLK_LCTRL) && event.key.key == SDLK_W) {
					running = 0;
				}

				if(event.key.key == SDLK_K) {
					camera_move(&cam, 0, 0, 0, 0.05, 0);
				}
				if(event.key.key == SDLK_M) {
					camera_move(&cam, 0, 0, 0, -0.05, 0);
				}
				if(event.key.key == SDLK_O) {
					camera_move(&cam, 0, 0, 0, 0, 0.05);
				}
				if(event.key.key == SDLK_L) {
					camera_move(&cam, 0, 0, 0, 0, -0.05);
				}
				if(event.key.key == SDLK_W) {
					camera_move(&cam, 0, 0, 0.5, 0, 0);	
				}
				if(event.key.key == SDLK_S) {
					camera_move(&cam, 0, 0, -0.5, 0, 0);
				}
				if(event.key.key == SDLK_A) {
					camera_move(&cam, -0.5, 0, 0, 0, 0);	
				}
				if(event.key.key == SDLK_D) {
					camera_move(&cam, 0.5, 0, 0, 0, 0);
				}
				if(event.key.key == SDLK_SPACE) {
					camera_move(&cam, 0, -0.5, 0, 0, 0);	
				}
				if(event.key.key == SDLK_LSHIFT) {
					camera_move(&cam, 0, 0.5, 0, 0, 0);
				}
				for(int i = 0; i < WIDTH * HEIGHT; i++) {
					framebuffer[i] = 0x2A2A2A;
				}

				camera_apply(&pro1, &cam, &pos1);
				camera_apply(&pro2, &cam, &pos2);
				camera_apply(&pro3, &cam, &pos3);
				camera_apply(&pro4, &cam, &pos4);
				camera_apply(&pro5, &cam, &pos5);
				camera_apply(&pro6, &cam, &pos6);
				camera_apply(&pro7, &cam, &pos7);
				camera_apply(&pro8, &cam, &pos8);
			}
		}

		set_pixel_vec(&pro1, 0xFFFFFF);
		set_pixel_vec(&pro2, 0xFFFFFF);
		set_pixel_vec(&pro3, 0xFFFFFF);
		set_pixel_vec(&pro4, 0xFFFFFF);
		set_pixel_vec(&pro5, 0xFFFFFF);
		set_pixel_vec(&pro6, 0xFFFFFF);
		set_pixel_vec(&pro7, 0xFFFFFF);
		set_pixel_vec(&pro8, 0xFFFFFF);

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
