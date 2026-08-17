#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "glad/glad.h"
#include "mat4d.h"
#include "vec4d.h"

struct camera {
	struct mat4d view;
	struct mat4d projection;
	struct vec4d pos;
	GLfloat yaw;
	GLfloat pitch;
};

void camera_projection(struct camera*, const GLfloat, const GLfloat, const GLfloat);
void camera_setpos(struct camera*, const GLfloat, const GLfloat, const GLfloat, const GLfloat, const GLfloat);
void camera_move(struct camera*, const GLfloat, const GLfloat, const GLfloat, const GLfloat, const GLfloat);
void camera_apply(struct vec4d*, struct camera*, const struct vec4d*);

#endif
