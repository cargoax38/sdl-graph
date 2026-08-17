#include "../headers/mat4d.h"
#include "glad/glad.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct mat4d* mat4d_create() {
	struct mat4d* mat = (struct mat4d*) malloc(sizeof(struct mat4d));
	if(mat == NULL) return NULL;

	for(int i = 0; i < 16; i++) {
		mat->elements[i] = 0;
	}

	return mat;
}

void mat4d_identity(struct mat4d* mat) {
	if(mat == NULL) return;

	mat->elements[0] = 1;
	mat->elements[1] = 0;
	mat->elements[2] = 0;
	mat->elements[3] = 0;
	mat->elements[4] = 0;
	mat->elements[5] = 1;
	mat->elements[6] = 0;
	mat->elements[7] = 0;
	mat->elements[8] = 0;
	mat->elements[9] = 0;
	mat->elements[10] = 1;
	mat->elements[11] = 0;
	mat->elements[12] = 0;
	mat->elements[13] = 0;
	mat->elements[14] = 0;
	mat->elements[15] = 1;
}

void mat4d_invert_view(struct mat4d* mat, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat yaw, const GLfloat pitch) {
	GLfloat cyaw = cos(yaw);
	GLfloat syaw = sin(yaw);
	GLfloat cpitch = cos(pitch);
	GLfloat spitch = sin(pitch);

	mat->elements[0] = cyaw;
	mat->elements[1] = 0;
	mat->elements[2] = syaw;
	mat->elements[3] = -x * cyaw - z * syaw;
	mat->elements[4] = -spitch * syaw;
	mat->elements[5] = cpitch;
	mat->elements[6] = spitch * cyaw;
	mat->elements[7] = x * spitch * syaw - y * cpitch - z * spitch * cyaw;
	mat->elements[8] = -cpitch * syaw;
	mat->elements[9] = -spitch;
	mat->elements[10] = cpitch * cyaw;
	mat->elements[11] = x * cpitch * syaw + y * spitch - z * cpitch * cyaw;
	mat->elements[12] = 0;
	mat->elements[13] = 0;
	mat->elements[14] = 0;
	mat->elements[15] = 1;
}

void mat4d_projection(struct mat4d* mat, const GLfloat fov, const GLfloat z_near, const GLfloat z_far) {
	if(mat == NULL) return;
	if(fov <= 0 || fov >= 3.14 || z_near == z_far) return;

	mat->elements[0] = (1 / tan(fov / 2));
	mat->elements[1] = 0;
	mat->elements[2] = 0;
	mat->elements[3] = 0;
	mat->elements[4] = 0;
	mat->elements[5] = (1 / tan(fov / 2));
	mat->elements[6] = 0;	
	mat->elements[7] = 0;
	mat->elements[8] = 0;
	mat->elements[9] = 0;
	mat->elements[10] = z_far / (z_far - z_near);
	mat->elements[11] = z_near * z_far / (z_near - z_far);
	mat->elements[12] = 0;
	mat->elements[13] = 0;
	mat->elements[14] = 1;
	mat->elements[15] = 0;
}

void mat4d_display(const struct mat4d* mat) {
	if(mat == NULL) return;

	for(int i = 0; i < 4; i++) {
		printf("| ");
		for(int j = 0; j < 4; j++) {
			double value = mat->elements[j + i * 4];
			if(value == -0) {
				printf("  0.00");
				continue;
			}

			if(value < 0) {
				printf(" %.2f", value);
			}else {
				printf("  %.2f", value);
			}
		}
		printf(" |\n");
	}
}

void mat4d_product(struct mat4d* res, const struct mat4d* mat1, const struct mat4d* mat2) {
	if(res == NULL || mat1 == NULL || mat2 == NULL) return;
	res->elements[0] = mat1->elements[0] * mat2->elements[0] + mat1->elements[1] * mat2->elements[4] + mat1->elements[2] * mat2->elements[8] + mat1->elements[3] * mat2->elements[12];
	res->elements[1] = mat1->elements[0] * mat2->elements[1] + mat1->elements[1] * mat2->elements[5] + mat1->elements[2] * mat2->elements[9] + mat1->elements[3] * mat2->elements[13];
	res->elements[2] = mat1->elements[0] * mat2->elements[2] + mat1->elements[1] * mat2->elements[6] + mat1->elements[2] * mat2->elements[10] + mat1->elements[3] * mat2->elements[14];
	res->elements[3] = mat1->elements[0] * mat2->elements[3] + mat1->elements[1] * mat2->elements[7] + mat1->elements[2] * mat2->elements[11] + mat1->elements[3] * mat2->elements[15];

	res->elements[4] = mat1->elements[4] * mat2->elements[0] + mat1->elements[5] * mat2->elements[4] + mat1->elements[6] * mat2->elements[8] + mat1->elements[7] * mat2->elements[12];
	res->elements[5] = mat1->elements[4] * mat2->elements[1] + mat1->elements[5] * mat2->elements[5] + mat1->elements[6] * mat2->elements[9] + mat1->elements[7] * mat2->elements[13];
	res->elements[6] = mat1->elements[4] * mat2->elements[2] + mat1->elements[5] * mat2->elements[6] + mat1->elements[6] * mat2->elements[10] + mat1->elements[7] * mat2->elements[14];
	res->elements[7] = mat1->elements[4] * mat2->elements[3] + mat1->elements[5] * mat2->elements[7] + mat1->elements[6] * mat2->elements[11] + mat1->elements[7] * mat2->elements[15];

	res->elements[8] = mat1->elements[8] * mat2->elements[0] + mat1->elements[9] * mat2->elements[4] + mat1->elements[10] * mat2->elements[8] + mat1->elements[11] * mat2->elements[12];
	res->elements[9] = mat1->elements[8] * mat2->elements[1] + mat1->elements[9] * mat2->elements[5] + mat1->elements[10] * mat2->elements[9] + mat1->elements[11] * mat2->elements[13];
	res->elements[10] = mat1->elements[8] * mat2->elements[2] + mat1->elements[9] * mat2->elements[6] + mat1->elements[10] * mat2->elements[10] + mat1->elements[11] * mat2->elements[14];
	res->elements[11] = mat1->elements[8] * mat2->elements[3] + mat1->elements[9] * mat2->elements[7] + mat1->elements[10] * mat2->elements[11] + mat1->elements[11] * mat2->elements[15];

	res->elements[12] = mat1->elements[12] * mat2->elements[0] + mat1->elements[13] * mat2->elements[4] + mat1->elements[14] * mat2->elements[8] + mat1->elements[15] * mat2->elements[12];
	res->elements[13] = mat1->elements[12] * mat2->elements[1] + mat1->elements[13] * mat2->elements[5] + mat1->elements[14] * mat2->elements[9] + mat1->elements[15] * mat2->elements[13];
	res->elements[14] = mat1->elements[12] * mat2->elements[2] + mat1->elements[13] * mat2->elements[6] + mat1->elements[14] * mat2->elements[10] + mat1->elements[15] * mat2->elements[14];
	res->elements[15] = mat1->elements[12] * mat2->elements[3] + mat1->elements[13] * mat2->elements[7] + mat1->elements[14] * mat2->elements[11] + mat1->elements[15] * mat2->elements[15];
}

void mat4d_apply_direct(const struct mat4d* mat, struct vec4d* vec) {
	double x = vec->elements[0];
	double y = vec->elements[1];
	double z = vec->elements[2];
	double w = vec->elements[3];

	vec->elements[0] = mat->elements[0] * x + mat->elements[1] * y + mat->elements[2] * z + mat->elements[3] * w;
	vec->elements[1] = mat->elements[4] * x + mat->elements[5] * y + mat->elements[6] * z + mat->elements[7] * w;
	vec->elements[2] = mat->elements[8] * x + mat->elements[9] * y + mat->elements[10] * z + mat->elements[11] * w;
	vec->elements[3] = mat->elements[12] * x + mat->elements[13] * y + mat->elements[14] * z + mat->elements[15] * w;
}

void mat4d_apply(struct vec4d* res, const struct mat4d* mat, const struct vec4d* vec) {
	res->elements[0] = mat->elements[0] * vec->elements[0] + mat->elements[1] * vec->elements[1] + mat->elements[2] * vec->elements[2] + mat->elements[3] * vec->elements[3];
	res->elements[1] = mat->elements[4] * vec->elements[0] + mat->elements[5] * vec->elements[1] + mat->elements[6] * vec->elements[2] + mat->elements[7] * vec->elements[3];
	res->elements[2] = mat->elements[8] * vec->elements[0] + mat->elements[9] * vec->elements[1] + mat->elements[10] * vec->elements[2] + mat->elements[11] * vec->elements[3];
	res->elements[3] = mat->elements[12] * vec->elements[0] + mat->elements[13] * vec->elements[1] + mat->elements[14] * vec->elements[2] + mat->elements[15] * vec->elements[3];
}

void mat4d_destroy(struct mat4d** mat) {
	if(mat == NULL) return;
	if(*mat == NULL) return;

	free(*mat);
	*mat = NULL;
}
