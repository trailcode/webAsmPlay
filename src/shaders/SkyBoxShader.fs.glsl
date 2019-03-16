#version 330 core
in vec3 texcoords;
uniform samplerCube cubeTexture;
out vec4 frag_colour;

void main()
{
	frag_colour = texture(cubeTexture, texcoords);
}
