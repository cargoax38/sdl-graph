#include "../headers/node.h"
#include "../headers/utils.h"

#include <glad/glad.h>
#include <math.h>
#include <stdlib.h>

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

GLuint nodeElementBuffer = 0;
GLuint nodeVertexBuffer = 0;
GLuint shaderProgram = 0;
GLuint uniform_l = 0;
GLuint uniform_x = 0;
GLuint uniform_y = 0;
GLuint uniform_z = 0;

int node_init_gl(const GLchar* vertexPath, const GLchar* fragmentPath) {
	shaderProgram = createShaderProgram(vertexPath, fragmentPath);
	if(shaderProgram == 0) return EXIT_FAILURE;

	uniform_l = glGetUniformLocation(shaderProgram, "l");
	uniform_x = glGetUniformLocation(shaderProgram, "x");
	uniform_y = glGetUniformLocation(shaderProgram, "y");
	uniform_z = glGetUniformLocation(shaderProgram, "z");

	glGenBuffers(1, &nodeElementBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), node_indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &nodeVertexBuffer);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), node_vertices, GL_STATIC_DRAW);

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

	glUniform1f(uniform_l, node->length);
	glUniform1f(uniform_x, node->x);
	glUniform1f(uniform_y, node->y);
	glUniform1f(uniform_z, node->z);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
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
