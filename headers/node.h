#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <glad/glad.h>

extern GLuint node_indexes[240];
extern GLfloat node_vertices[126];
extern GLuint nodeElementBuffer, nodeVertexBuffer;
extern GLuint shaderProgram;
extern GLuint uniform_t;
extern GLuint uniform_l;
extern GLuint uniform_x;
extern GLuint uniform_y;
extern GLuint uniform_z;

struct node {
	GLfloat length;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat dx;
	GLfloat dy;
	GLfloat dz;
};

int node_init_gl(const GLchar* vertexPath, const GLchar* fragmentPath);
void node_free_gl();

struct node* node_init(GLfloat length, GLfloat x, GLfloat y, GLfloat z);
void node_pre_draw();
void node_draw(struct node* node);
void node_post_draw();
void node_clean(struct node* node);

#endif
