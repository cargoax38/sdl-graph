#include "../headers/camera.h"
#include <math.h>
#include <stddef.h>

void camera_projection(struct camera* cam, const double fov, const double z_near, const double z_far) {
	mat4d_projection(&(cam->projection), fov, z_near, z_far);
}

void camera_setpos(struct camera* cam, const double x, const double y, const double z, const double yaw, const double pitch) {
	cam->pos = vec4d_init(x, y, z);
	cam->yaw = yaw;
	cam->pitch = pitch;

	mat4d_invert_view(&(cam->view), x, y, z, yaw, pitch);
}

void camera_move(struct camera* cam, const double dx, const double dy, const double dz, const double dyaw, const double dpitch) {
	cam->yaw += dyaw;
	cam->pitch += dpitch;
	if(cam->pitch < -1.5707) cam->pitch = -1.5707;
	if(cam->pitch > 1.5707) cam->pitch = 1.5707;

	cam->pos.elements[0] += dx * cos(cam->yaw) - dz * sin(cam->yaw);
	cam->pos.elements[1] += dy;
	cam->pos.elements[2] += dx * sin(cam->yaw) + dz * cos(cam->yaw);

	mat4d_invert_view(&(cam->view), cam->pos.elements[0], cam->pos.elements[1], cam->pos.elements[2], cam->yaw, cam->pitch);
}

void camera_apply(struct vec4d* res, struct camera* cam, const struct vec4d* vec) {
	struct mat4d tran;

	mat4d_product(&tran, &(cam->projection), &(cam->view));

	mat4d_apply(res, &tran, vec);
}
