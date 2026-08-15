#include "headers/utils.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 512
#define HEIGHT 512

void* test(const char* c) {
	return SDL_GL_GetProcAddress(c);
}

SDL_Window* window = NULL;
SDL_GLContext context = NULL;
SDL_Event event;

GLuint program;
GLuint vertexArrayObject;
GLuint vertexBufferObjects[2];

uint8_t createSDLContext();
uint8_t createGLContext();
void destroyApplication();

GLuint compileShader(GLuint type, const GLchar* source) {
	GLuint shaderObject;

	if(type == GL_VERTEX_SHADER) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	glShaderSource(shaderObject, 1, &source, NULL);
	glCompileShader(shaderObject);

	return shaderObject;
}

GLuint createShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath) {
	char* vertexSource = readFile(vertexPath);

	if(vertexSource == NULL) {
		return 0;
	}

	char* fragmentSource = readFile(fragmentPath);

	if(fragmentSource == NULL) {
		free(vertexSource);
		return 0;
	}

	GLuint program = glCreateProgram();

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	free(vertexSource);
	free(fragmentSource);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glValidateProgram(program);

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void initDraw(GLuint* running) {
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_EVENT_QUIT) {
			*running = 0;
		}
	}
}

void preDraw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.2F, 0.2F, 0.2F, 1.0F);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void draw(GLuint program) {
	glUseProgram(program);

	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

int main(void) {
	if(createSDLContext() == EXIT_FAILURE) return EXIT_FAILURE;
	if(createGLContext() == EXIT_FAILURE) return EXIT_FAILURE;

	const GLfloat vertexPosition[9] = {
		-0.8F, -0.8F, 0.0F,
		0.8F, -0.8F, 0.0F,
		0.0F, 0.8F, 0.0F
	};

	const GLfloat vertexColor[9] = {
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F
	};

	vertexArrayObject = 0;
	vertexBufferObjects[0] = 0;
	vertexBufferObjects[1] = 0;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(2, vertexBufferObjects);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexPosition, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexColor, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	GLuint graphicsPipeline = createShaderProgram("..\\res\\vertex.glsl", "..\\res\\fragment.glsl");
	if(graphicsPipeline == 0) {
		destroyApplication();
		return EXIT_FAILURE;
	}

	GLuint running = 1;
	while(running) {		
		initDraw(&running);
		preDraw();
		draw(graphicsPipeline);
		
		SDL_GL_SwapWindow(window);
	}

	destroyApplication();

	return EXIT_SUCCESS;
}

uint8_t createSDLContext() {
	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Error when initializing the video");
		return EXIT_FAILURE;
	}

	window = SDL_CreateWindow("SDL with OpenGL", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if(window == NULL) {
		SDL_Quit();
		fprintf(stderr, "Error when initializing the window");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

uint8_t createGLContext() {
	context = SDL_GL_CreateContext(window);
	if(context == NULL) {
		fprintf(stderr, "Error when creating the OpenGL Context");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	if(!gladLoadGLLoader(test)) {
		fprintf(stderr, "Error when initializing GLAD");
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// OpenGL 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	printf("%s\n", glGetString(GL_VENDOR));
	printf("%s\n", glGetString(GL_RENDERER));
	printf("%s\n", glGetString(GL_VERSION));
	printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	return EXIT_SUCCESS;
}

void destroyApplication() {
	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
