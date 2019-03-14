#version 330 core

layout(location = 0) in vec3 vertIn;

uniform mat4   model;
uniform mat4   view;
uniform mat4   projection;
uniform float  heightMultiplier;

out vec4 glPos;

void main()
{
	vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

	gl_Position = projection * view * model * vert;

	glPos = gl_Position;
}
