#version 300 es
//#version 330 core
precision mediump float;
in vec3 texcoords;
uniform samplerCube cubeTexture;
out vec4 frag_colour;

void main()
{
	frag_colour = texture(cubeTexture, texcoords);
}
