#include "../headers/node.h"
#include "../headers/utils.h"

#include <glad/glad.h>
#include <math.h>
#include <stdlib.h>

/*
GLuint node_indexes[36] = {
	5, 1, 7,
	7, 1, 3,

	1, 0, 3,
	3, 0, 2,

	0, 4, 2,
	2, 4, 6,

	4, 5, 6,
	6, 5, 7,
	
	4, 0, 5,
	5, 0, 1,

	7, 3, 6,
	6, 3, 2
};

GLfloat node_vertices[24] = {
	-0.5F, -0.5F, -0.5F,
	-0.5F, -0.5F, 0.5F,
	-0.5F, 0.5F, -0.5F,
	-0.5F, 0.5F, 0.5F,
	0.5F, -0.5F, -0.5F,
	0.5F, -0.5F, 0.5F,
	0.5F, 0.5F, -0.5F,
	0.5F, 0.5F, 0.5F
};
*/

GLuint node_indexes[240] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5,
	0, 5, 1,

	1, 6, 7,
	1, 7, 2,
	2, 7, 8,
	2, 8, 9,
	2, 9, 3,
	3, 9, 10,
	3, 10, 11,
	3, 11, 4,
	4, 11, 12,
	4, 12, 13,
	4, 13, 5,
	5, 13, 14,
	5, 14, 15,
	5, 15, 1,
	1, 15, 6,

	6, 25, 16,
	6, 16, 7,
	7, 16, 17,
	7, 17, 8,
	8, 17, 18,
	8, 18, 9,
	9, 18, 19,
	9, 19, 10,
	10, 19, 20,
	10, 20, 11,
	11, 20, 21,
	11, 21, 12,
	12, 21, 22,
	12, 22, 13,
	13, 22, 23,
	13, 23, 14,
	14, 23, 24,
	14, 24, 15,
	15, 24, 25,
	15, 25, 6,

	16, 35, 26,
	16, 26, 17,
	17, 26, 27,
	17, 27, 18,
	18, 27, 28,
	18, 28, 19,
	19, 28, 29,
	19, 29, 20,
	20, 29, 30,
	20, 30, 21,
	21, 30, 31,
	21, 31, 22,
	22, 31, 32,
	22, 32, 23,
	23, 32, 33,
	23, 33, 24,
	24, 33, 34,
	24, 34, 25,
	25, 34, 35,
	25, 35, 16,

	26, 36, 27,
	27, 36, 37,
	27, 37, 28,
	28, 37, 29,
	29, 37, 38,
	29, 38, 30,
	30, 38, 31,
	31, 38, 39,
	31, 39, 32,
	32, 39, 33,
	33, 39, 40,
	33, 40, 34,
	34, 40, 35,
	35, 40, 36,
	35, 36, 26,

	36, 41, 37,
	37, 41, 38,
	38, 41, 39,
	39, 41, 40,
	40, 41, 36
};

GLfloat node_vertices[126] = {
	+0.00F, +1.00F, +0.00F,

	+0.53F, +0.85F, +0.00F,
	+0.16F, +0.85F, +0.50F,
	-0.43F, +0.85F, +0.31F,
	-0.43F, +0.86F, -0.31F,
	+0.16F, +0.86F, -0.50F,

	+0.89F, +0.45F, +0.00F,
	+0.69F, +0.53F, +0.50F,
	+0.28F, +0.45F, +0.85F,
	-0.26F, +0.53F, +0.81F,
	-0.72F, +0.45F, +0.53F,
	-0.85F, +0.53F, +0.00F,
	-0.72F, +0.45F, -0.53F,
	-0.26F, +0.53F, -0.81F,
	+0.28F, +0.45F, -0.85F,
	+0.69F, +0.53F, -0.50F,	
	
	+0.95F, +0.00F, +0.31F,
	+0.59F, +0.00F, +0.81F,
	+0.00F, +0.00F, +1.00F,
	-0.59F, +0.00F, +0.81F,
	-0.95F, +0.00F, +0.31F,
	-0.95F, +0.00F, -0.31F,
	-0.59F, +0.00F, -0.81F,
	+0.00F, +0.00F, -1.00F,
	+0.59F, +0.00F, -0.81F,
	+0.95F, +0.00F, -0.31F,

	+0.72F, -0.45F, +0.53F,
	+0.26F, -0.53F, +0.81F,
	-0.28F, -0.45F, +0.85F,
	-0.69F, -0.53F, +0.50F,
	-0.89F, -0.45F, +0.00F,
	-0.69F, -0.53F, -0.50F,
	-0.28F, -0.45F, -0.85F,
	+0.26F, -0.53F, -0.81F,
	+0.72F, -0.45F, -0.53F,
	+0.85F, -0.53F, +0.00F,

	+0.43F, -0.85F, +0.31F,
	-0.16F, -0.85F, +0.50F,
	-0.53F, -0.85F, +0.00F,
	-0.16F, -0.85F, -0.50F,
	+0.43F, -0.85F, -0.31F,

	+0.00F, -1.00F, +0.00F
};

GLuint nodeElementBuffer = 0;
GLuint nodeVertexBuffer = 0;
GLuint shaderProgram = 0;
GLuint uniform_t = 0;
GLuint uniform_l = 0;
GLuint uniform_x = 0;
GLuint uniform_y = 0;
GLuint uniform_z = 0;

GLfloat time;

int node_init_gl(const GLchar* vertexPath, const GLchar* fragmentPath) {
	shaderProgram = createShaderProgram(vertexPath, fragmentPath);
	if(shaderProgram == 0) return EXIT_FAILURE;

	uniform_t = glGetUniformLocation(shaderProgram, "t");
	uniform_l = glGetUniformLocation(shaderProgram, "l");
	uniform_x = glGetUniformLocation(shaderProgram, "x");
	uniform_y = glGetUniformLocation(shaderProgram, "y");
	uniform_z = glGetUniformLocation(shaderProgram, "z");

	time = 0;

	glGenBuffers(1, &nodeElementBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 240 * sizeof(GLuint), node_indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &nodeVertexBuffer);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 126 * sizeof(GLfloat), node_vertices, GL_STATIC_DRAW);

	glBindVertexArray(nodeVertexBuffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glDisableVertexAttribArray(0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);

	return EXIT_SUCCESS;
}

void node_free_gl() {
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &nodeElementBuffer);
	glDeleteVertexArrays(1, &nodeVertexBuffer);
}

struct node* node_init(GLfloat length, GLfloat x, GLfloat y, GLfloat z) {
	struct node* node = (struct node*) malloc(sizeof(struct node));
	if(node == NULL) return NULL;

	node->length = length;
	node->x = x;
	node->y = y;
	node->z = z;
	node->dx = 0;
	node->dy = 0;
	node->dz = 0;

	return node;
}

void node_pre_draw() {
	glUseProgram(shaderProgram);
	glBindVertexArray(nodeVertexBuffer);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeElementBuffer);
}

void node_draw(struct node* node) {
	if(node == NULL) return;

	glUniform1f(uniform_t, time);
	glUniform1f(uniform_l, node->length);
	glUniform1f(uniform_x, node->x);
	glUniform1f(uniform_y, node->y);
	glUniform1f(uniform_z, node->z);
	
	glDrawElements(GL_TRIANGLES, 240, GL_UNSIGNED_INT, (GLvoid*)0);

	time += 0.01667;
}

void node_post_draw() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void node_clean(struct node* node) {
	if(node == NULL) return;

	free(node);
}
