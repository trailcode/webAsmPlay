#include <vector>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/OpenGL_Util.h>

GLuint dynamicCubeTex = 0;

GLuint framebuffer = 0;

GLuint depthRenderbuffer = 0;

const int cubeMapSize = 1024; // Size of the cube map texture image, width and height

void cubeMapInit()
{
	if(framebuffer) { return ;}

	// Create empty cubemap
	glGenTextures(1, &dynamicCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Allocate space for each side of the cube map
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubeMapSize,
			cubeMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	// Create framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapSize, cubeMapSize);
	// Attach one of the faces of the cubemap texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);
	// Attach depth buffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
	// Attach only the +X cubemap texture (for completeness)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);

	// Check if current configuration of framebuffer is correct
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		dmessError("Framebuffer not complete!");

	// Set default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

