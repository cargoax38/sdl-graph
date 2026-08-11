#include "../headers/vec4d.h"

struct vec4d vec4d_init(const double x, const double y, const double z) {
	return (struct vec4d) {.elements = {x, y, z, 1.0}};
}
