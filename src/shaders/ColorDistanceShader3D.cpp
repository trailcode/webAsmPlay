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

#include <webAsmPlay/Types.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>

using namespace std;
using namespace glm;

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram         * shaderProgramFill    = NULL;
    ShaderProgram         * shaderProgramOutline = NULL;
    ColorDistanceShader3D * defaultInstance      = NULL;

    GLint vertInAttrFill;
    GLint normalInAttrFill;
    GLint vertColorInAttrFill;

    GLint colorLookupOffsetFill;
    GLint heightMultiplierFill;
    GLint modelFill;
    GLint viewFill;
    GLint projectionFill;
    GLint texUniformFill;
    GLint lightPosUniformFill;

    GLint vertInAttrOutline;
    GLint vertColorInAttrOutline;
    GLint colorLookupOffsetOutline;
    GLint MV_Outline;
    GLint MVP_Outline;
    GLint texUniformOutline;
    GLint heightMultiplierOutline;
}

void ColorDistanceShader3D::ensureShader()
{
	shaderProgramFill = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,		"ColorDistanceShader3D_fill.vs.glsl"	},
																{GL_FRAGMENT_SHADER,	"ColorDistanceShader3D_fill.fs.glsl"	}}),
													Variables({	{"vertIn",               vertInAttrFill							},
																{"vertColorIn",          vertColorInAttrFill					},
																{"normalIn",             normalInAttrFill						}}),
													Variables({	{"tex",                  texUniformFill							},
																{"model",                modelFill								},
																{"view",                 viewFill								},
																{"projection",           projectionFill							},
																{"colorLookupOffset",    colorLookupOffsetFill					},
																{"heightMultiplier",     heightMultiplierFill					},
																{"lightPos",             lightPosUniformFill					}}));

	shaderProgramOutline = ShaderProgram::create(   GLSL({		{GL_VERTEX_SHADER,		"ColorDistanceShader3D_outline.vs.glsl"	},
																{GL_FRAGMENT_SHADER,	"ColorDistanceShader3D_outline.fs.glsl"	}}),
                                                    Variables({	{"vertIn",				vertInAttrOutline						},
																{"vertColorIn",			vertColorInAttrOutline					}}),
                                                    Variables({	{"MV",					MV_Outline								},
																{"MVP",					MVP_Outline								},
																{"tex",					texUniformOutline						},
																{"colorLookupOffset",	colorLookupOffsetOutline				},
																{"heightMultiplier",	heightMultiplierOutline					}}));

    defaultInstance = new ColorDistanceShader3D();
}

ColorDistanceShader3D::ColorDistanceShader3D() : Shader("ColorDistanceShader3D")
{
}

ColorDistanceShader3D::~ColorDistanceShader3D()
{

}

ColorDistanceShader3D * ColorDistanceShader3D::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceShader3D::bind(Canvas     * canvas,
                                 const bool   isOutline,
                                 const size_t renderingStage)
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID()));

    if(!isOutline)
    {
        shaderProgramFill->bind();

        shaderProgramFill->setUniformi(texUniformFill,          0);
        shaderProgramFill->setUniformf(heightMultiplierFill,    m_heightMultiplier);
        shaderProgramFill->setUniform (modelFill,               canvas->getModelRef());
        shaderProgramFill->setUniform (viewFill,                canvas->getViewRef());
        shaderProgramFill->setUniform (projectionFill,          canvas->getProjectionRef());
        shaderProgramFill->setUniform (lightPosUniformFill,     m_lightPos);
        shaderProgramFill->setUniformf(colorLookupOffsetFill,   0.0f);
    }
    else
    {
        shaderProgramOutline->bind();

        shaderProgramOutline->setUniformf(heightMultiplierOutline,  m_heightMultiplier);
        shaderProgramOutline->setUniform(MV_Outline,                canvas->getMV_Ref());
        shaderProgramOutline->setUniform(MVP_Outline,               canvas->getMVP_Ref());
        shaderProgramOutline->setUniformi(texUniformOutline,        0);
        shaderProgramOutline->setUniformf(colorLookupOffsetOutline, 1.0f);
    }
}

float ColorDistanceShader3D::setHeightMultiplier(const float multiplier) { return m_heightMultiplier = multiplier	;}
float ColorDistanceShader3D::getHeightMultiplier() const				 { return m_heightMultiplier				;}

vec3 ColorDistanceShader3D::setLightPos(const vec3 & pos) { return m_lightPos = pos	;}
vec3 ColorDistanceShader3D::getLightPos() const			  { return m_lightPos		;}

ColorSymbology * ColorDistanceShader3D::setColorSymbology(ColorSymbology * colorSymbology)	{ return m_colorSymbology = colorSymbology	;}
ColorSymbology * ColorDistanceShader3D::getColorSymbology() const							{ return m_colorSymbology					;}