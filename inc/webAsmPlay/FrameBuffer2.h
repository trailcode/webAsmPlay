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
#pragma once

#include <utility>
#include <vector>
#include <glm/vec2.hpp>
#include <webAsmPlay/OpenGL_Util.h>

typedef std::pair<GLenum, GLenum> TexParam;

struct FB_Component
{
	FB_Component(	const GLenum				  type,
					const GLenum				  dataType,
					const std::vector<TexParam> & textureParameters = std::vector<TexParam>()) :	m_type				(type),
																									m_dataType			(dataType),
																									m_textureParameters	(textureParameters) {}

	const GLenum m_type;
	const GLenum m_dataType;

	const std::vector<TexParam> m_textureParameters;
};

class FrameBuffer2
{
public:

	FrameBuffer2(const glm::ivec2					& bufferSize,
				 const std::vector<FB_Component>	& components);
								
	~FrameBuffer2();

	glm::ivec2 getBufferSize() const;

	void bind(const bool clear = true);
	void unbind();

	GLuint getTextureID(const size_t component = 0) const;

private:

	glm::ivec2 m_bufferSize;

	GLuint m_renderFBO;

	std::vector<GLuint> m_textures;

	std::vector<GLenum> m_drawBuffers;

	GLint m_prevFB = 0;

private:
};