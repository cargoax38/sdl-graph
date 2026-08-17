#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform float time;

out vec4 v_color;
out vec3 v_position;
out vec3 v_lightDir;

void main() {
	mat4 rotZ = transpose(mat4(cos(time / 4), 0, -sin(time / 4), 0, 0, 1, 0, 0.2 * sin(time / 2), sin(time / 4), 0, cos(time / 4), 0, 0, 0, 0, 1));
	gl_Position = proj * view * rotZ * position;

	v_color = color;
	v_position = gl_Position.xyz;
	v_lightDir = vec3(0, 0, -1);
}
