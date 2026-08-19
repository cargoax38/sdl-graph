#version 410 core

layout(location = 0) in vec4 position;

uniform float l;
uniform float x;
uniform float y;
uniform float z;

out float v_color;

void main() {
	float theta = 0.2;
	mat4 rotZ = transpose(mat4(l * cos(theta), 0, -l * sin(theta), x, 0, l, 0, y, l * sin(theta), 0, l * cos(theta), z, 0, 0, 0, 1));
	mat4 tran = transpose(mat4(1.43, 0, 0, 0, 0, 1.43, 0, 0, 0, 0, 1.01, -0.10 + 2, 0, 0, 1, +2));

	gl_Position = tran * rotZ * position;

	v_color = distance(gl_Position.xyz, vec3(0, 0, 4)) / 9.9F;
}
