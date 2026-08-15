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

const char* vertexSource =
	"#version 410 core\n"
	"in vec4 position;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
	"}\n";

const char* fragmentSource =
	"#version 410 core\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"	color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
	"}\n";

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

GLuint createShaderProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
	GLuint program = glCreateProgram();

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

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

int main(void) {
	if(createSDLContext() == EXIT_FAILURE) return EXIT_FAILURE;
	if(createGLContext() == EXIT_FAILURE) return EXIT_FAILURE;

	printf("%s\n", glGetString(GL_VENDOR));
	printf("%s\n", glGetString(GL_RENDERER));
	printf("%s\n", glGetString(GL_VERSION));
	printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	const GLfloat vertexPosition[9] = {
		-0.8F, -0.8F, 0.0F,
		0.8F, -0.8F, 0.0F,
		0.0F, 0.8F, 0.0F
	};

	GLuint vertexArrayObject = 0;
	GLuint vertexBufferObject = 0;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexPosition, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	
	FILE* f;
	errno_t err;
	char* res;

	if((err = fopen_s(&f, "res/fragment.glsl", "r")) != 0) {
		fprintf(stderr, "Error when reading GLSL fragment shader");
	}else {
		fseek(f, 0, SEEK_END);
		long length = ftell(f);
		fseek(f, 0, SEEK_SET);
		res = (char*) malloc(length);

		fread(res, 1, length, f);
		fclose(f);
	}

	GLuint graphicsPipeline = createShaderProgram(vertexSource, res);

	uint8_t running = 1;
	while(running) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUseProgram(graphicsPipeline);

		glBindVertexArray(vertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

		glDrawArrays(GL_LINE_LOOP, 0, 3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		SDL_GL_SwapWindow(window);
	}

	free(res);
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
	
	return EXIT_SUCCESS;
}

void destroyApplication() {
	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
