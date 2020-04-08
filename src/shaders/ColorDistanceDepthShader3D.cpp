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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>

using namespace std;
using namespace glm;

//REGISTER_SHADER(ColorDistanceDepthShader3D)

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram              * a_shaderProgramPre		= nullptr;
    ShaderProgram              * a_shaderProgramFill	= nullptr;
    ShaderProgram              * a_shaderProgramOutline	= nullptr;
    ColorDistanceDepthShader3D * a_defaultInstance		= nullptr;

    GLint a_vertInAttrPre;
    GLint a_heightMultiplierPre;
    
    GLint a_vertInAttrFill;
    GLint a_normalInAttrFill;
    GLint a_vertColorInAttrFill;

    GLint a_colorLookupOffsetFill;
    GLint a_heightMultiplierFill;
	GLint a_invPersMatrixFill;
	GLint a_invViewMatrixFiLL;
    GLint a_colorLookupTextureUniformFill;
	GLint a_topDownTextureUniformFill;
    GLint a_depthTexUniformFill;
    GLint a_lightPosUniformFill;
   
    GLint a_vertInAttrOutline;
    GLint a_vertColorInAttrOutline;
    GLint a_colorLookupOffsetOutline;
    GLint a_texUniformOutline;
    GLint a_depthTexUniformOutline;
    GLint a_heightMultiplierOutline;
}

void ColorDistanceDepthShader3D::ensureShader()
{
	if(a_defaultInstance) { return ;}

	a_shaderProgramPre = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_pre.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_pre.fs.glsl"		}}),
													Variables({	{"vertIn",					a_vertInAttrPre									}}),
													Variables({	{"heightMultiplier",		a_heightMultiplierPre							}}));

	a_shaderProgramFill = ShaderProgram::create(	GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_fill.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_fill.fs.glsl"		}}),
													Variables({	{"vertIn",					a_vertInAttrFill								},
																{"vertColorIn",				a_vertColorInAttrFill							},
																{"normalIn",				a_normalInAttrFill								}}),
													Variables({	{"colorLookupTexture",		a_colorLookupTextureUniformFill					},
																{"topDownTexture",			a_topDownTextureUniformFill						},
																{"depthTex",				a_depthTexUniformFill							},
																{"invPersMatrix",			a_invPersMatrixFill								},
																{"invViewMatrix",			a_invViewMatrixFiLL								},
																{"colorLookupOffset",		a_colorLookupOffsetFill							},
																{"heightMultiplier",		a_heightMultiplierFill							},
																{"lightPos",				a_lightPosUniformFill							}}));

	a_shaderProgramOutline = ShaderProgram::create( GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_outline.vs.glsl"	},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_outline.fs.glsl"	}}),
                                                    Variables({	{"vertIn",					a_vertInAttrOutline								},
																{"vertColorIn",				a_vertColorInAttrOutline						}}),
                                                    Variables({	{"tex",						a_texUniformOutline								},
																{"depthTex",				a_depthTexUniformOutline						},
																{"colorLookupOffset",		a_colorLookupOffsetOutline						},
																{"heightMultiplier",		a_heightMultiplierOutline						}}));

    a_defaultInstance = new ColorDistanceDepthShader3D();
}

ColorDistanceDepthShader3D::ColorDistanceDepthShader3D(ColorSymbology * colorSymbology) : Shader	("ColorDistanceDepthShader3D",
																									colorSymbology ? colorSymbology : ColorSymbology::getInstance("defaultMesh"),
																									[](const bool isOutline, const size_t renderingStage) -> bool
																									{
																										if(renderingStage == POST_G_BUFFER) { return true ;}

																										if(renderingStage == G_BUFFER && !isOutline) { return true ;}

																										return false;
																									})
{
}

ColorDistanceDepthShader3D::~ColorDistanceDepthShader3D()
{

}

ColorDistanceDepthShader3D * ColorDistanceDepthShader3D::getDefaultInstance() { return a_defaultInstance ;}

void ColorDistanceDepthShader3D::bind(Canvas     * canvas,
                                      const bool   isOutline,
                                      const size_t renderingStage)
{
    switch(renderingStage)
    {
        case POST_G_BUFFER: return bindStagePostG_Buffer(canvas, isOutline);
        case G_BUFFER:		return bindStageG_Buffer	(canvas, isOutline);
        default: 
            dmessError("Error!");
    }
}

void ColorDistanceDepthShader3D::bindStageG_Buffer(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStageG_Buffer");

    a_shaderProgramPre->bind();

	glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    a_shaderProgramPre->setUniformf(a_heightMultiplierPre, m_heightMultiplier);
}

void ColorDistanceDepthShader3D::bindStagePostG_Buffer(Canvas * canvas, const bool isOutline)
{
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID());
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, canvas->getG_FrameBuffer()->getTextureID(1));

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
        a_shaderProgramFill->bind();
        
        a_shaderProgramFill->setUniformi(a_colorLookupTextureUniformFill,	0);
        a_shaderProgramFill->setUniformi(a_depthTexUniformFill,				1);
		a_shaderProgramFill->setUniformi(a_topDownTextureUniformFill,		2);
        a_shaderProgramFill->setUniformf(a_heightMultiplierFill,			m_heightMultiplier);
		a_shaderProgramFill->setUniform (a_invPersMatrixFill,				inverse(canvas->getProjectionRef()));
		a_shaderProgramFill->setUniform (a_invViewMatrixFiLL,				inverse(canvas->getViewRef()));
        a_shaderProgramFill->setUniform (a_lightPosUniformFill,				m_lightPos);
        a_shaderProgramFill->setUniformf(a_colorLookupOffsetFill,			0.0f);
    }
    else
    {
        a_shaderProgramOutline->bind();
        
        a_shaderProgramOutline->setUniformf(a_heightMultiplierOutline,		m_heightMultiplier);
        a_shaderProgramOutline->setUniformi(a_depthTexUniformOutline,		1);
        a_shaderProgramOutline->setUniformi(a_texUniformOutline,			0);
        a_shaderProgramOutline->setUniformf(a_colorLookupOffsetOutline,		1.0f);
    }
}

float ColorDistanceDepthShader3D::setHeightMultiplier(const float multiplier)	{ return m_heightMultiplier = multiplier ;}
float ColorDistanceDepthShader3D::getHeightMultiplier() const					{ return m_heightMultiplier				 ;}

vec3 ColorDistanceDepthShader3D::setLightPos(const vec3 & pos)					{ return m_lightPos = pos ;}
vec3 ColorDistanceDepthShader3D::getLightPos() const							{ return m_lightPos		  ;}

size_t ColorDistanceDepthShader3D::getNumRenderingStages() const				{ return 2 ;}

#endif