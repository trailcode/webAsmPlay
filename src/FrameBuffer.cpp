﻿/**
╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

\author Matthew Tang
\email trailcode@gmail.com
\copyright 2019
*/

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/FrameBuffer.h>

using namespace std;
using namespace glm;

FrameBuffer::FrameBuffer(	const ivec2					& bufferSize,
							const vector<FB_Component>	& components) : m_components(components)
{
	m_textures.resize(m_components.size());

	initFrameBuffer(bufferSize);
}

FrameBuffer::~FrameBuffer() { cleanup() ;}

ivec2 FrameBuffer::initFrameBuffer(const ivec2& bufferSize)
{
	m_bufferSize = bufferSize;

	m_drawBuffers.clear();

#ifndef __EMSCRIPTEN__

	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_prevFB);

	glGenFramebuffers(1,				&m_renderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,	 m_renderFBO);

	glGenTextures(GLsizei(m_components.size()), &m_textures[0]);

	for (size_t i = 0; i < m_components.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glTexStorage2D(GL_TEXTURE_2D, 1, m_components[i].m_dataType, m_bufferSize.x, m_bufferSize.y);

		for (const auto& texParam : m_components[i].m_textureParameters) { glTexParameteri(GL_TEXTURE_2D, get<0>(texParam), get<1>(texParam)) ;}

		glFramebufferTexture(GL_FRAMEBUFFER, m_components[i].m_type, m_textures[i], 0);

		if (m_components[i].m_type != GL_DEPTH_ATTACHMENT) { m_drawBuffers.push_back(m_components[i].m_type) ;}
	}

	glDrawBuffers(GLsizei(m_drawBuffers.size()), &m_drawBuffers[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, m_prevFB);

	return bufferSize;

#endif

	glGenFramebuffers(1, &m_renderFBO);
    //glGenTextures(1, t);
	glGenTextures(GLsizei(m_components.size()), &m_textures[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, m_renderFBO);

    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_bufferSize.x, m_bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[0], 0);

    GLuint depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);    
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_bufferSize.x, m_bufferSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
	{
        dmessError("Error!");
	}

	return m_bufferSize;
}

void FrameBuffer::cleanup()
{
	unbind();

	glDeleteTextures(GLsizei(m_textures.size()), &m_textures[0]);

	glDeleteFramebuffers(1, &m_renderFBO);
}

ivec2 FrameBuffer::getBufferSize() const { return m_bufferSize ;}

ivec2 FrameBuffer::setBufferSize(const ivec2& bufferSize)
{
	if (bufferSize == m_bufferSize) { return bufferSize; }

	cleanup();

	return initFrameBuffer(bufferSize);
}

void FrameBuffer::bind(const bool clear)
{
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_prevFB);

	glBindFramebuffer(GL_FRAMEBUFFER, m_renderFBO);

	glGetIntegerv(GL_VIEWPORT, m_prevViewport);

	glViewport(0, 0, m_bufferSize.x, m_bufferSize.y);

	if(!clear) { return ;}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_prevFB);

	glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
}

GLuint FrameBuffer::getTextureID(const size_t component) const { return m_textures[component] ;}
