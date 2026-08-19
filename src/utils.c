#include "../headers/utils.h"

#include <stdio.h>
#include <stdlib.h>

GLchar* readFile(const GLchar* path) {
	char* text = NULL;

	FILE* file;
	errno_t error;

	// Read binary mode
	if((error = fopen_s(&file, path, "rb")) != 0) {
		fprintf(stderr, "Error when reading the file %s, error code : %d", path, error);
	}else {
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		rewind(file);
		//fseek(file, 0, SEEK_SET);
		text = (char*) malloc((length + 1) * sizeof(char));

		if(text == NULL) {
			fclose(file);
			fprintf(stderr, "Error when allocating memory space for the file %s", path);
			return NULL;
		}

		fread(text, 1, length, file);
		text[length] = '\0';
		fclose(file);
	}

	return text;
}

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

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
