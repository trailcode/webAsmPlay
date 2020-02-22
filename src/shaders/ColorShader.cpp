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

#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ColorShader.h>

using namespace glm;

//REGISTER_SHADER(ColorShader)

namespace
{
    ShaderProgram * a_shaderProgram   = nullptr;
    ColorShader   * a_defaultInstance = nullptr;

    GLint a_vertInAttr;
    GLint a_colorInUniform;

    GLint a_MVP;
}

void ColorShader::ensureShader()
{
    if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"ColorShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"ColorShader.fs.glsl"	}}),
                                            Variables({	{"vertIn",				a_vertInAttr			}}),
                                            Variables({	{"MVP",					a_MVP					},
														{"colorIn",				a_colorInUniform		}}));

    a_defaultInstance = new ColorShader(Shader::s_defaultShouldRender);
}

ColorShader * ColorShader::getDefaultInstance() { return a_defaultInstance ;}

ColorShader::ColorShader(const ShouldRenderFunctor & shouldRenderFunctor) : Shader(	"ColorShader",
																					ColorSymbology::getInstance("defaultPolygon"),
																					shouldRenderFunctor),
																			m_fillColor		(0,1,0,0.5),
																			m_outlineColor	(1,1,0,1)
{
}

ColorShader::~ColorShader()
{

}

void ColorShader::bind( Canvas     * canvas,
                        const bool   isOutline,
                        const size_t renderingStage)
{
	bind(canvas->getMVP_Ref(), isOutline);
}

void ColorShader::bind(	const mat4		& model,
						const mat4		& view,
						const mat4		& projection,
						const bool		  isOutline,
						const size_t	  renderingStage)
{
	bind(projection * view * model, isOutline);
}

void ColorShader::bind(	const mat4	& MVP,
						const bool	  isOutline)
{
	a_shaderProgram->bind();

    a_shaderProgram->setUniform(a_MVP, MVP);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ONE);

    if(isOutline) { a_shaderProgram->setUniform(a_colorInUniform, m_outlineColor) ;}
    else          { a_shaderProgram->setUniform(a_colorInUniform, m_fillColor)    ;}
}