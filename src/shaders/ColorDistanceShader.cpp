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
	ShaderProgram       * shaderProgramPre	= nullptr;
    ShaderProgram       * shaderProgram		= nullptr;
    ColorDistanceShader * defaultInstance   = nullptr;

    GLint vertInAttrLoc;
    GLint vertColorInAttrLoc;

    GLint colorLookupOffsetLoc;
    GLint MV_Loc;
    GLint MVP_Loc;
    GLint texUniformLoc;
	GLint topDownTextureUniform;
}

void ColorDistanceShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({		{GL_VERTEX_SHADER,		"ColorDistanceShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"ColorDistanceShader.fs.glsl"	}}),
											Variables({	{"vertIn",				vertInAttrLoc					},
														{"vertColorIn",			vertColorInAttrLoc				}}),
											Variables({	{"MV",					MV_Loc							},
														{"MVP",					MVP_Loc							},
														{"tex",					texUniformLoc					},
														{"topDownTexture",		topDownTextureUniform			},
														{"colorLookupOffset",	colorLookupOffsetLoc			}}));

    defaultInstance = new ColorDistanceShader();
}

ColorDistanceShader::ColorDistanceShader() : Shader("ColorDistanceShader",
													nullptr,
													// Should render functor
													[](const bool isOutline, const size_t renderingStage) -> bool
													{
														return renderingStage == 1;
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

ColorDistanceShader * ColorDistanceShader::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceShader::bind(Canvas     * canvas,
                               const bool   isOutline,
                               const size_t renderingStage)
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

	shaderProgram->bind();

	shaderProgram->setUniform(MV_Loc,  canvas->getMV_Ref());
	shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());

	shaderProgram->setUniformi(texUniformLoc,			0);
	shaderProgram->setUniformi(topDownTextureUniform,	1);

    if(isOutline) { shaderProgram->setUniformf(colorLookupOffsetLoc, 2.0f) ;}
    else          { shaderProgram->setUniformf(colorLookupOffsetLoc, 0.0f) ;}
}

size_t ColorDistanceShader::getNumRenderingStages() const { return 2 ;}
