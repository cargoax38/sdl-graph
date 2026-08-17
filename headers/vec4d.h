#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "glad/glad.h"
struct vec4d {
	GLfloat elements[4];
};

struct vec4d vec4d_init(const GLfloat, const GLfloat, const GLfloat);

#endif
