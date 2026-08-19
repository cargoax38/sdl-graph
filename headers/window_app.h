#ifndef WINDOW_APP_INCLUDED
#define WINDOW_APP_INCLUDED

#include <glad/glad.h>
#include <SDL3/SDL.h>

struct Window {
	SDL_Window* window;
	SDL_GLContext context;
	SDL_Event event;
	GLuint width;
	GLuint height;
};

struct Window* window_init(const GLchar* title, const GLuint width, const GLuint height);
void window_free(struct Window* win);

#endif
