#version 410 core

layout(location = 0) in vec4 position;

uniform float t;
uniform float l;
uniform float x;
uniform float y;
uniform float z;

out vec3 v_position;
out vec3 v_lightDir;

void main() {
	mat4 rotX = transpose(mat4(1, 0, 0, 0, 0, cos(2 * t), -sin(2 * t), 0, 0, sin(2 * t), cos(2 * t), 0, 0, 0, 0, 1));
	mat4 rotZ = transpose(mat4(cos(t), 0, -sin(t), 0, 0, 1, 0, 0, sin(t), 0, cos(t), 0, 0, 0, 0, 1));
	mat4 tran = transpose(mat4(l, 0, 0, x, 0, l, 0, y, 0, 0, l, z, 0, 0, 0, 1));
	mat4 proj = transpose(mat4(1.43, 0, 0, 0, 0, 1.43, 0, 0, 0, 0, 1.01, -0.10 + 3, 0, 0, 1, +3));

	gl_Position = proj * tran * rotZ * rotX * position;

	v_position = gl_Position.xyz;
	v_lightDir = vec3(0, 0, -1);
}
