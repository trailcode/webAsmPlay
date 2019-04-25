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

#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/SsaoShader.h>

//REGISTER_SHADER(SsaoShader)

namespace
{
	ShaderProgram * shaderProgram   = NULL;
	SsaoShader	  * defaultInstance = NULL;
}

SsaoShader* SsaoShader::getDefaultInstance() { return defaultInstance ;}



void SsaoShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({	{GL_VERTEX_SHADER,		"SsaoShader.vs.glsl"	},
													{GL_FRAGMENT_SHADER,	"SsaoShader.fs.glsl"	}}),
											Variables({}),
											Variables({}));

	defaultInstance = new SsaoShader();
}

SsaoShader::SsaoShader() : Shader("SsaoShader")
{

}

SsaoShader::~SsaoShader()
{

}

GLuint SsaoShader::setColorTextureID(const GLuint textureID) { return m_colorTextureID = textureID ;}

void SsaoShader::bind(	Canvas		* canvas,
						const bool    isOutline,
						const size_t  renderingStage)
{
	shaderProgram->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorTextureID);
}