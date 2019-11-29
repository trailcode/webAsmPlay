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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>

using namespace std;
using namespace glm;

//REGISTER_SHADER(ColorDistanceShader)

namespace
{
	ShaderProgram       * a_shaderProgramPre	= nullptr;
    ShaderProgram       * a_shaderProgram		= nullptr;
    ColorDistanceShader * a_defaultInstance   = nullptr;

    GLint a_vertInAttr;
    GLint a_vertColorInAttr;

    GLint a_colorLookupOffset;
    GLint a_MV;
    GLint a_MVP;
    GLint a_texUniform;
	GLint a_topDownTextureUniform;
}

void ColorDistanceShader::ensureShader()
{
	if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"ColorDistanceShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"ColorDistanceShader.fs.glsl"	}}),
											Variables({	{"vertIn",				a_vertInAttr					},
														{"vertColorIn",			a_vertColorInAttr				}}),
											Variables({	{"MV",					a_MV							},
														{"MVP",					a_MVP							},
														{"tex",					a_texUniform					},
														{"topDownTexture",		a_topDownTextureUniform			},
														{"colorLookupOffset",	a_colorLookupOffset				}}));

    a_defaultInstance = new ColorDistanceShader();
}

ColorDistanceShader::ColorDistanceShader() : Shader("ColorDistanceShader",
													ColorSymbology::getInstance("defaultPolygon"),
													// Should render functor
													[](const bool isOutline, const size_t renderingStage) -> bool
													{
														return renderingStage == G_BUFFER;
													})
{
}

ColorDistanceShader::ColorDistanceShader(	ColorSymbology				* colorSymbology,
											const ShouldRenderFunctor	& shouldRenderFunctor) : Shader("ColorDistanceShader",
																										colorSymbology,
																										shouldRenderFunctor)
{

}

ColorDistanceShader::~ColorDistanceShader()
{

}

ColorDistanceShader * ColorDistanceShader::getDefaultInstance() { return a_defaultInstance ;}

void ColorDistanceShader::bind(Canvas     * canvas,
                               const bool   isOutline,
                               const size_t renderingStage)
{
	bind(canvas->getMV_Ref(), canvas->getMVP_Ref(), isOutline);
}

void ColorDistanceShader::bind(	const mat4		& model,
								const mat4		& view,
								const mat4		& projection,
								const bool		  isOutline,
								const size_t	  renderingStage)
{
	const auto MV	= view		 * model;
    const auto MVP	= projection * MV;

	bind(MV, MVP, isOutline);
}

void ColorDistanceShader::bind(	const mat4		& MV,
								const mat4		& MVP,
								const bool		  isOutline)
{
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID());

	glActiveTexture(GL_TEXTURE1);

	if (RenderableBingMap::getFrameBuffer())
	{
		glBindTexture(GL_TEXTURE_2D, RenderableBingMap::getFrameBuffer()->getTextureID());
	}
	else
	{
		dmess("Fix!"); // Happens if big maps is not enabled.
	}

	a_shaderProgram->bind();

	a_shaderProgram->setUniform(a_MV,  MV);
	a_shaderProgram->setUniform(a_MVP, MVP);

	a_shaderProgram->setUniformi(a_texUniform,				0);
	a_shaderProgram->setUniformi(a_topDownTextureUniform,	1);

    if(isOutline) { a_shaderProgram->setUniformf(a_colorLookupOffset, 2.0f) ;}
    else          { a_shaderProgram->setUniformf(a_colorLookupOffset, 0.0f) ;}
}

size_t ColorDistanceShader::getNumRenderingStages() const { return 2 ;}
