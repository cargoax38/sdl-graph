#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 v_color;

void main() {
	gl_Position = vec4(position.x, position.y, position.z, position.w);

	v_color = color;
}
