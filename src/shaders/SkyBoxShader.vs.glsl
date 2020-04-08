#version 300 es
//#version 330 core
in vec3 vertIn;
uniform mat4 MVP; 
out vec3 texcoords;

void main()
{
	texcoords = vertIn;

	gl_Position = MVP * vec4(vertIn, 1.0);
}

