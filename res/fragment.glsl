#version 410 core

in vec4 v_color;
in vec3 v_position;
in vec3 v_lightDir;

out vec4 color;

void main() {
	vec3 dx = dFdx(v_position);
	vec3 dy = dFdy(v_position);

	vec3 N = normalize(cross(dx, dy));
	vec3 L = normalize(-v_lightDir);

	float diffuse = max(dot(N, L), 0);

	color = vec4(diffuse * v_color.xyz, 1.0);
}
