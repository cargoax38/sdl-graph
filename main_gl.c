#include "headers/camera.h"
#include "headers/utils.h"
#include "headers/mat4d.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 768
#define HEIGHT 768
#define RATIO 6
#define IFPS 0.0167

void* test(const char* c) {
	return SDL_GL_GetProcAddress(c);
}

SDL_Window* window = NULL;
SDL_GLContext context = NULL;
SDL_Event event;

GLuint program;
GLuint vertexArrayObject;
GLuint vertexBufferObject;
GLuint elementBufferObject;
GLuint frameBufferObject;

GLuint uniformView;
GLuint uniformProj;
GLuint uniformTime;

struct camera cam;

uint8_t keys[6] = {0, 0, 0, 0, 0, 0};

uint8_t createSDLContext();
uint8_t createGLContext();
void destroyApplication();

static void GLClearAllErrors() {
	while(glGetError() != GL_NO_ERROR) {}
}

static uint8_t GLCheckErrorStatus(const char* function, int line) {
	GLenum error;
	while((error = glGetError())) {
		printf("OpenGL Error : %d\n\tLine : %d\n\tFunction : %s\n", error, line, function);
		return 1;
	}
	return 0;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

GLuint compileShader(GLuint type, const GLchar* source) {
	GLuint shaderObject;

	if(type == GL_VERTEX_SHADER) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	if(shaderObject == (GLuint)0) {
		fprintf(stderr, "Error when creating the shader object");
		return 0;
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
	if(vertexShader == 0 || fragmentShader == 0) {
		fprintf(stderr, "Problems when initializing shaders");
	}

	free(vertexSource);
	free(fragmentSource);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glValidateProgram(program);

	uniformView = glGetUniformLocation(program, "view");
	uniformProj = glGetUniformLocation(program, "proj");
	uniformTime = glGetUniformLocation(program, "time");

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
}

void preDraw() {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, WIDTH / RATIO, HEIGHT / RATIO);
	glClearColor(0.2F, 0.2F, 0.2F, 1.0F);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void draw(GLuint program, double frame) {
	glUseProgram(program);

	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

	glUniformMatrix4fv(uniformView, 1, GL_TRUE, (GLfloat*) cam.view.elements);
	glUniformMatrix4fv(uniformProj, 1, GL_TRUE, (GLfloat*) cam.projection.elements);
	glUniform1f(uniformTime, frame);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(
		0, 0, WIDTH / RATIO, HEIGHT / RATIO,
		0, 0, WIDTH, HEIGHT,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
}

int main(void) {
	if(createSDLContext() == EXIT_FAILURE) return EXIT_FAILURE;
	if(createGLContext() == EXIT_FAILURE) return EXIT_FAILURE;

	camera_setpos(&cam, 0, 0, -4, 0, 0);
	camera_projection(&cam, 1.22F, 0.1F, 10.0F);

	GLuint colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        WIDTH / RATIO,
        HEIGHT / RATIO,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        0
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint depthStencil;
    glGenRenderbuffers(1, &depthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencil);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        WIDTH / RATIO,
        HEIGHT / RATIO
    );

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorTexture,
        0
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        depthStencil
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Incomplete framebuffer");
		destroyApplication();
        return EXIT_FAILURE;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Making an icosaedron
	// X Y Z R G B
	const GLfloat vertexPosition[72] = {
		0.0F, 0.95F, 0.0F, 1.0F, 0.0F, 0.0F,

		0.69, 0.43, 0.5, 1.0F, 0.0F, 0.0F,
		-0.27, 0.43, 0.81, 1.0F, 0.0F, 0.0F,
		-0.86, 0.43, 0, 1.0F, 0.0F, 0.0F,
		-0.26, 0.43, -0.81, 1.0F, 0.0F, 0.0F,
		0.69, 0.43, -0.5, 1.0F, 0.0F, 0.0F,

		0.86, -0.43, 0, 1.0F, 0.0F, 0.0F, 
		0.26, -0.43, 0.81, 1.0F, 0.0F, 0.0F,
		-0.69, -0.43, 0.5, 1.0F, 0.0F, 0.0F,
		-0.69, -0.43, -0.5, 1.0F, 0.0F, 0.0F,
		0.26, -0.43, -0.81, 1.0F, 0.0F, 0.0F,

		0.0F, -0.95F, 0.0F, 1.0F, 0.0F, 0.0F
	};

	const GLuint indexVertex[60] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 1,

		1, 6, 7,
		1, 7, 2,
		2, 7, 8,
		2, 8, 3,
		3, 8, 9,
		3, 9, 4,
		4, 9, 10,
		4, 10, 5,
		5, 10, 6,
		5, 6, 1,

		7, 6, 11,
		8, 7, 11,
		9, 8, 11,
		10, 9, 11,
		6, 10, 11
	};

	vertexArrayObject = 0;
	vertexBufferObject = 0;
	elementBufferObject = 0;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 60 * sizeof(GLint), indexVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), vertexPosition, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));

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
	uint64_t begin, end;
	double elapsed;

	double time = 0;

	while(running) {
		begin = SDL_GetPerformanceCounter();

		initDraw(&running);
		preDraw();
		draw(graphicsPipeline, time);
		
		SDL_GL_SwapWindow(window);

		end = SDL_GetPerformanceCounter();

		elapsed = (double)(end - begin) / (double)SDL_GetPerformanceFrequency();
		if(elapsed < IFPS) {
			SDL_Delay(1000.0 * (IFPS - elapsed));
		}
		time += 0.10;
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
