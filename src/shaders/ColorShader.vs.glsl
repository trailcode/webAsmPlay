#version 300 es
//#version 330 core

//layout(location = 0) in vec3 vertIn;
in vec3 vertIn;

out vec4 vertexColor;
uniform mat4 MVP;
uniform vec4 colorIn;

void main()
{
	gl_Position = MVP * vec4(vertIn.xyz, 1);
	vertexColor = colorIn;
}