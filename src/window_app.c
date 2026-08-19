#include "../headers/window_app.h"
#include <glad/glad.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

void* sdl_GetProcAddress(const char* c) {
	return SDL_GL_GetProcAddress(c);
}

struct Window* window_init(const GLchar* title, const GLuint width, const GLuint height) {
	struct Window* win = (struct Window*) malloc(sizeof(struct Window));
	if(win == NULL) {
		return NULL;
	}
	SDL_Event event;
	win->event = event;

	win->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
	if(win->window == NULL) {
		free(win);
		fprintf(stderr, "Error when initializing the SDL window\n");
		return NULL;
	}

	win->context = SDL_GL_CreateContext(win->window);
	if(win->context == NULL) {
		SDL_DestroyWindow(win->window);
		free(win);
		fprintf(stderr, "Error when initializing the OpenGL context\n");
		return NULL;
	}

	if(!gladLoadGLLoader(sdl_GetProcAddress)) {
		fprintf(stderr, "Error when initializing GLAD");

		SDL_GL_DestroyContext(win->context);
		SDL_DestroyWindow(win->window);
		free(win);
		return NULL;
	}

	win->width = width;
	win->height = height;

	return win;
}

void window_free(struct Window* win) {
	if(win == NULL) return;

	SDL_GL_DestroyContext(win->context);
	SDL_DestroyWindow(win->window);
	free(win);
}

