#version 300 es
//#version 330 core
//layout(location = 0) in vec3 vertIn;
//layout(location = 1) in vec4 vertColorIn;
in vec3 vertIn;
in vec4 vertColorIn;
out vec4 vertexColor;
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertIn.xyz, 1);
	vertexColor = vertColorIn;
}