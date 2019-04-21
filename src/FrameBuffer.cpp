/**
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

	GL_CHECK(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_prevFB));

	GL_CHECK(glGenFramebuffers(1,				&m_renderFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER,	 m_renderFBO));

	glGenTextures(m_components.size(), &m_textures[0]);

	for (size_t i = 0; i < m_components.size(); ++i)
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textures[i]));

		GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, m_components[i].m_dataType, m_bufferSize.x, m_bufferSize.y));

		for (const auto& texParam : m_components[i].m_textureParameters) { GL_CHECK(glTexParameteri(GL_TEXTURE_2D, get<0>(texParam), get<1>(texParam))) ;}

		GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, m_components[i].m_type, m_textures[i], 0));

		if (m_components[i].m_type != GL_DEPTH_ATTACHMENT) { m_drawBuffers.push_back(m_components[i].m_type) ;}
	}

	GL_CHECK(glDrawBuffers(m_drawBuffers.size(), &m_drawBuffers[0]));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_prevFB));

	return bufferSize;
}

void FrameBuffer::cleanup()
{
	unbind();

	GL_CHECK(glDeleteTextures(m_textures.size(), &m_textures[0]));

	GL_CHECK(glDeleteFramebuffers(1, &m_renderFBO));
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
	GL_CHECK(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_prevFB));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_renderFBO));

	if(!clear) { return ;}

	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void FrameBuffer::unbind() { GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_prevFB)); }

GLuint FrameBuffer::getTextureID(const size_t component) const { return m_textures[component] ;}
