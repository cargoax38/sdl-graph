#include "../headers/camera.h"
#include <math.h>


void camera_projection(struct camera* cam, const double fov, const double z_near, const double z_far) {
	mat4d_projection(&(cam->projection), fov, z_near, z_far);
}

void camera_setpos(struct camera* cam, const double x, const double y, const double z, const double yaw, const double pitch) {
	mat4d_invert_view(&(cam->transform), x, y, z, yaw, pitch);
}

void camera_move(struct camera* cam, const double dx, const double dy, const double dz, const double dyaw, const double dpitch) {
	struct mat4d mat;
	struct mat4d res;

	mat4d_invert_view(&mat, dx, dy, dz, dyaw, dpitch);
	
	mat4d_product(&res, &mat, &(cam->transform));
	cam->transform = res;
}

void camera_apply(struct vec4d* res, struct camera* cam, const struct vec4d* vec) {
	struct mat4d tran;
	mat4d_product(&tran, &(cam->projection), &(cam->transform));

	mat4d_apply(res, &tran, vec);
}
