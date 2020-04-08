#version 300 es
//#version 330 core


precision mediump float;
//layout (location = 0) out vec4 outColor;
out vec4 outColor;

in vec4 vertexColor;

void main()
{
	outColor = vertexColor;
}
