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
\copyright 2018
*/

#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/TextureLookupShader.h>

REGISTER_SHADER(TextureLookupShader)

namespace
{
	ShaderProgram		* shaderProgram		= NULL;
	TextureLookupShader * defaultInstance	= NULL;

	GLint vertInAttr;
	GLint MVP_Loc;
}

void TextureLookupShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(	GLSL({		{GL_VERTEX_SHADER,		"TextureLookupShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"TextureLookupShader.fs.glsl"	},
														{GL_GEOMETRY_SHADER,	"TextureLookupShader.gs.glsl"	}}),
											Variables({	{"vertIn",				vertInAttr						}}),
											Variables({	{"MVP",					MVP_Loc							}}));

	defaultInstance = new TextureLookupShader();
}

TextureLookupShader* TextureLookupShader::getDefaultInstance() { return defaultInstance ;}

TextureLookupShader::TextureLookupShader() : Shader("TextureLookupShader")
{

}

TextureLookupShader::~TextureLookupShader()
{

}

void TextureLookupShader::bind( Canvas     * canvas,
								const bool   isOutline,
								const size_t renderingStage)
{
	shaderProgram->bind();

	shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());
}

GLuint64 TextureLookupShader::setTextureHandle(const GLuint64& handle) { return m_texHandle = handle; }