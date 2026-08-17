#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "glad/glad.h"
#include "vec4d.h"

struct mat4d {
	GLfloat elements[16];
};

/**
 * Creates an matrix assimilated to 0_{4} "struct mat4d" on the heap.
 *
 * @return The address of that matrix.
 */
struct mat4d* mat4d_create();

/**
 * Changes the coefficients of a matrix to make it equal to I_{4}.
 *
 * @param struct mat4d* mat : the matrix.
 */
void mat4d_identity(struct mat4d*);

/**
 * Changes the coefficients of a matrix to make it equal to the invert of a view matrix.
 *
 * @params struct mat4d* mat : the matrix.
 * @params const double x : the x position of the camera.
 * @params const double y : the y position of the camera.
 * @params const double z : the z position of the camera.
 * @params const double yaw : the yaw rotation of the camera (rotation about the y canonical coordinate of R^{3}).
 * @params const double pitch : the pitch rotation of the camera (rotation about the x axis after applying the yaw rotation).
 */
void mat4d_invert_view(struct mat4d*, const GLfloat, const GLfloat, const GLfloat, const GLfloat, const GLfloat);

/**
 * Changes the coefficients of a matrix to make it equal to a projection matrix along the z axis.
 *
 * @params struct mat4d* mat : the matrix.
 * @params double fov : the field of view of the camera.
 * @params double z_near : the near plane of the camera.
 * @params double z_far : the far plane of the camera.
 */
void mat4d_projection(struct mat4d*, const GLfloat, const GLfloat, const GLfloat);

/**
 * Shows a matrix on the standard output.
 *
 * @params const struct mat4d* : the matrix.
 */
void mat4d_display(const struct mat4d*);

/**
 * Compute the product of two matrices and put the result on a third matrix.
 *
 * @params struct mat4d* res : the matrix where the output will be.
 * @params const struct mat4d* mat1 : the matrix on the left side of the times operator.
 * @params const struct mat4d* mat2 : the matrix on the right side of the times operator.
 */
void mat4d_product(struct mat4d*, const struct mat4d*, const struct mat4d*);

/**
 * Apply the operation MX where M is a matrix and X is a vector and directly put the result into X.
 *
 * @params const struct mat4d* mat : the matrix.
 * @params struct vec4d* vec : the vector.
 */
void mat4d_apply_direct(const struct mat4d*, struct vec4d*);

/**
 * Apply the operation MX where M is a matrix and X is a vector on a third vector.
 *
 * @params struct vec4d* res : the vector where the result of the product will be stored.
 * @params const struct mat4d* mat : the matrix.
 * @params const struct vec4d* vec : the vector.
 */
void mat4d_apply(struct vec4d*, const struct mat4d*, const struct vec4d*);

/**
 * Destroy a matrix created on the heap.
 *
 * @params const struct mat4d** map : the address of the address of a matrix.
 */
void mat4d_destroy(struct mat4d**);

#endif
