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

#ifndef __EMSCRIPTEN__

#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>

//REGISTER_SHADER(BindlessTextureShader)

using namespace glm;

namespace
{
	ShaderProgram			* a_shaderProgram   = nullptr;
	BindlessTextureShader	* a_defaultInstance = nullptr;

	GLint a_vertInAttr;
	GLint a_vertUV_InAttr;

	GLint a_MVP;
	GLint a_texID;
}

BindlessTextureShader* BindlessTextureShader::getDefaultInstance() { return a_defaultInstance ;}

void BindlessTextureShader::ensureShader()
{
	if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"BindlessTextureShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"BindlessTextureShader.fs.glsl"	}}),
											Variables({	{"vertIn",				a_vertInAttr					},
														{"vertUV_In",			a_vertUV_InAttr					}}),
											Variables({	{"MVP",					a_MVP							},
														{"texID",				a_texID							}}));

	a_defaultInstance = new BindlessTextureShader([](const bool isOutline, const size_t renderingStage) -> bool
	{
		return renderingStage == G_BUFFER;
	});
}

void BindlessTextureShader::bind(	Canvas		* canvas,
									const bool    isOutline,
									const size_t  renderingStage)
{
	a_shaderProgram->bind();

	a_shaderProgram->setUniformi(a_texID, m_textureSlot);

	a_shaderProgram->setUniform(a_MVP, canvas->getMVP_Ref());
}

void BindlessTextureShader::bind(	const mat4		& model,
									const mat4		& view,
									const mat4		& projection,
									const bool		  isOutline,
									const size_t	  renderingStage)
{
	a_shaderProgram->bind();

	a_shaderProgram->setUniformi(a_texID, m_textureSlot);

	a_shaderProgram->setUniform(a_MVP, projection * view * model);
}

BindlessTextureShader::BindlessTextureShader(const ShouldRenderFunctor & shouldRender) : Shader("BindlessTextureShader",
																								ColorSymbology::getInstance("defaultPolygon"),
																								shouldRender)
{

}

BindlessTextureShader::~BindlessTextureShader()
{

}

size_t BindlessTextureShader::setTextureSlot(const size_t textureSlot)
{
	return m_textureSlot = (GLuint)textureSlot;
}

#endif