#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

struct vec4d {
	double elements[4];
};

struct vec4d vec4d_init(const double, const double, const double);

#endif
