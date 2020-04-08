#version 300 es
//#version 330 core

precision mediump float;

in  vec4 vertexColor;

out vec4 outColor;

void main()
{
	outColor = vertexColor;
	//outColor = vec4(1,1,0,0.5);
	//outColor = vec4(0,0,0,1);
}
