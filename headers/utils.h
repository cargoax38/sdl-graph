#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "glad/glad.h"

GLchar* readFile(const GLchar*);
GLuint compileShader(GLuint type, const GLchar* source);
GLuint createShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath);

#endif
