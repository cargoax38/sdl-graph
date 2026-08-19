#version 410 core

in float v_color;
out vec4 color;

void main() {
	color = vec4(v_color, v_color, v_color, 1.0F);
}
