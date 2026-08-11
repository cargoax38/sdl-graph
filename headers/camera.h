#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "mat4d.h"
#include "vec4d.h"

struct camera {
	struct mat4d transform;
	struct mat4d projection;
};

void camera_projection(struct camera*, const double, const double, const double);
void camera_setpos(struct camera*, const double, const double, const double, const double, const double);
void camera_move(struct camera*, const double, const double, const double, const double, const double);
void camera_apply(struct vec4d*, struct camera*, const struct vec4d*);

#endif
