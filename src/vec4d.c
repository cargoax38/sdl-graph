#include "../headers/vec4d.h"
#include "glad/glad.h"

struct vec4d vec4d_init(const GLfloat x, const GLfloat y, const GLfloat z) {
	return (struct vec4d) {.elements = {x, y, z, (GLfloat)1.0}};
}
