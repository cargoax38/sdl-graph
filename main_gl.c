#include "headers/node.h"
#include "headers/window_app.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 768
#define HEIGHT 768
#define IFPS 0.0166667

struct Window* window;

int main(void) {
	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Error when initializing the video");
		return EXIT_FAILURE;
	}

	window = window_init("Test Window", WIDTH, HEIGHT);

	if(window == NULL) {
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if(node_init_gl("..\\res\\vert.glsl", "..\\res\\frag.glsl") == EXIT_FAILURE) {
		window_free(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	struct node* cube = node_init(0.2, 1, 1, 0);

	bool running = true;
	uint64_t begin, end;
	GLdouble elapsed;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
	
	node_pre_draw();

	GLfloat x = cube->x;
	GLfloat y = cube->y;
	GLfloat z = cube->z;
	GLfloat dx = 0;
	GLfloat dy = 0;
	GLfloat dz = 0;

	GLfloat k = 30;
	GLfloat a = 10;

	GLfloat l = 0;

	while(running) {
		begin = SDL_GetPerformanceCounter();

		while(SDL_PollEvent(&(window->event))) {
			if((window->event).type == SDL_EVENT_QUIT) {
				running = false;
			}

			if((window->event).type == SDL_EVENT_KEY_DOWN) {
				if((window->event).key.key == SDLK_W) {
					cube->x += 0.01;
				}
			}
		}

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		node_draw(cube);

		l = x * x + y * y + z * z;

		if((fabs(dx) >= 0.01 || fabs(dy) >= 0.01 || fabs(dz) >= 0.01) || l > 0.01) {
			x = cube->x + IFPS * cube->dx;
			y = cube->y + IFPS * cube->dy;
			z = cube->z + IFPS * cube->dz;

			dx = cube->dx - IFPS * (k * cube->x + a * cube->dx);
			dy = cube->dy - IFPS * (k * cube->y + a * cube->dy);
			dz = cube->dz - IFPS * (k * cube->z + a * cube->dz);

			cube->x = x;
			cube->y = y;
			cube->z = z;
			cube->dx = dx;
			cube->dy = dy;
			cube->dz = dz;
		}

		SDL_GL_SwapWindow(window->window);

		end = SDL_GetPerformanceCounter();
		elapsed = (GLdouble)(end - begin) / (GLdouble)SDL_GetPerformanceFrequency();

		if(elapsed < IFPS) {
			SDL_Delay(1000 * (IFPS - elapsed));
		}
	}

	node_post_draw();
	node_free_gl();

	window_free(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
