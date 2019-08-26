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
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>

using namespace std;
using namespace glm;

//REGISTER_SHADER(ColorDistanceDepthShader3D)

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram              * shaderProgramPre		= nullptr;
    ShaderProgram              * shaderProgramFill		= nullptr;
    ShaderProgram              * shaderProgramOutline	= nullptr;
    ColorDistanceDepthShader3D * defaultInstance		= nullptr;

    GLint vertInAttrPre;
    GLint heightMultiplierPre;
    
    GLint vertInAttrFill;
    GLint normalInAttrFill;
    GLint vertColorInAttrFill;

    GLint colorLookupOffsetFill;
    GLint heightMultiplierFill;
	GLint invPersMatrixFill;
	GLint invViewMatrixFiLL;
    GLint colorLookupTextureUniformFill;
	GLint topDownTextureUniformFill;
    GLint depthTexUniformFill;
    GLint lightPosUniformFill;
   
    GLint vertInAttrOutline;
    GLint vertColorInAttrOutline;
    GLint colorLookupOffsetOutline;
    GLint texUniformOutline;
    GLint depthTexUniformOutline;
    GLint heightMultiplierOutline;
}

void ColorDistanceDepthShader3D::ensureShader()
{
	if(defaultInstance) { return ;}

	shaderProgramPre = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_pre.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_pre.fs.glsl"		}}),
													Variables({	{"vertIn",					vertInAttrPre									}}),
													Variables({	{"heightMultiplier",		heightMultiplierPre								}}));

	shaderProgramFill = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_fill.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_fill.fs.glsl"		}}),
													Variables({	{"vertIn",					vertInAttrFill									},
																{"vertColorIn",				vertColorInAttrFill								},
																{"normalIn",				normalInAttrFill								}}),
													Variables({	{"colorLookupTexture",		colorLookupTextureUniformFill					},
																{"topDownTexture",			topDownTextureUniformFill						},
																{"depthTex",				depthTexUniformFill								},
																{"invPersMatrix",			invPersMatrixFill								},
																{"invViewMatrix",			invViewMatrixFiLL								},
																{"colorLookupOffset",		colorLookupOffsetFill							},
																{"heightMultiplier",		heightMultiplierFill							},
																{"lightPos",				lightPosUniformFill								}}));

	shaderProgramOutline = ShaderProgram::create(   GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_outline.vs.glsl"	},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_outline.fs.glsl"	}}),
                                                    Variables({	{"vertIn",					vertInAttrOutline								},
																{"vertColorIn",				vertColorInAttrOutline							}}),
                                                    Variables({	{"tex",						texUniformOutline								},
																{"depthTex",				depthTexUniformOutline							},
																{"colorLookupOffset",		colorLookupOffsetOutline						},
																{"heightMultiplier",		heightMultiplierOutline							}}));

    defaultInstance = new ColorDistanceDepthShader3D();
}

ColorDistanceDepthShader3D::ColorDistanceDepthShader3D() : Shader("ColorDistanceDepthShader3D")
{
}

ColorDistanceDepthShader3D::~ColorDistanceDepthShader3D()
{

}

ColorDistanceDepthShader3D * ColorDistanceDepthShader3D::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceDepthShader3D::bind(Canvas     * canvas,
                                      const bool   isOutline,
                                      const size_t renderingStage)
{
    switch(renderingStage)
    {
        case 0: return bindStage0(canvas, isOutline);
        case 1: return bindStage1(canvas, isOutline);
        default: 
            dmessError("Error!");
    }
}

void ColorDistanceDepthShader3D::bindStage1(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage1");

    shaderProgramPre->bind();

	glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    shaderProgramPre->setUniformf(heightMultiplierPre, m_heightMultiplier);
}

void ColorDistanceDepthShader3D::bindStage0(Canvas * canvas, const bool isOutline)
{
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID());

    glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, canvas->getG_FrameBuffer()->getTextureID(1));

	glActiveTexture(GL_TEXTURE2);

	if (RenderableBingMap::getFrameBuffer())
	{
		glBindTexture(GL_TEXTURE_2D, RenderableBingMap::getFrameBuffer()->getTextureID());
	}
	else
	{
		dmess("Fix!!!"); // TODO
	}

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
	
    if(!isOutline)
    {
        shaderProgramFill->bind();
        
        shaderProgramFill->setUniformi(colorLookupTextureUniformFill,   0);
        shaderProgramFill->setUniformi(depthTexUniformFill,				1);
		shaderProgramFill->setUniformi(topDownTextureUniformFill,		2);
        shaderProgramFill->setUniformf(heightMultiplierFill,			m_heightMultiplier);
		shaderProgramFill->setUniform(invPersMatrixFill,				inverse(canvas->getProjectionRef()));
		shaderProgramFill->setUniform(invViewMatrixFiLL,				inverse(canvas->getViewRef()));
        shaderProgramFill->setUniform (lightPosUniformFill,				m_lightPos);
        shaderProgramFill->setUniformf(colorLookupOffsetFill,			0.0f);
    }
    else
    {
        shaderProgramOutline->bind();
        
        shaderProgramOutline->setUniformf(heightMultiplierOutline,		m_heightMultiplier);
        shaderProgramOutline->setUniformi(depthTexUniformOutline,		1);
        shaderProgramOutline->setUniformi(texUniformOutline,			0);
        shaderProgramOutline->setUniformf(colorLookupOffsetOutline,		1.0f);
    }
}

bool ColorDistanceDepthShader3D::shouldRender(const bool isOutline, const size_t renderingStage) const
{
    if(renderingStage == 0) { return true ;}

    if(renderingStage == 1 && !isOutline) { return true ;}

    return false;
}

float ColorDistanceDepthShader3D::setHeightMultiplier(const float multiplier)	{ return m_heightMultiplier = multiplier ;}
float ColorDistanceDepthShader3D::getHeightMultiplier() const					{ return m_heightMultiplier				 ;}

vec3 ColorDistanceDepthShader3D::setLightPos(const vec3 & pos)					{ return m_lightPos = pos ;}
vec3 ColorDistanceDepthShader3D::getLightPos() const							{ return m_lightPos		  ;}

size_t ColorDistanceDepthShader3D::getNumRenderingStages() const				{ return 2 ;}
