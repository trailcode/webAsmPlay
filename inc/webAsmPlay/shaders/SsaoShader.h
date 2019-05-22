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

#include <webAsmPlay/shaders/Shader.h>

class SsaoShader : public Shader
{
public:
	
	static SsaoShader * getDefaultInstance();

	SsaoShader();
	~SsaoShader();

	static void ensureShader();

	void bind(	Canvas     * canvas,
				const bool   isOutline,
				const size_t renderingStage) override;

	GLuint setColorTextureID		(const GLuint textureID);
	GLuint setNormalDepthTextureID	(const GLuint textureID);

	float setSSAO_Radius(const float radius);
	float getSSAO_Radius() const;

	GLuint setNumPoints(const GLuint num);
	GLuint getNumPoints() const;

	float setMinDepth(const float minDepth);
	float getMinDepth() const;

private:

	GLuint m_colorTextureID			= 0;
	GLuint m_normalDepthTextureID	= 0;

	float m_SSAO_Radius = 0.005;

	GLuint m_numPoints = 64;

	float m_minDepth = 0.001;
	
};
