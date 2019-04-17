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
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>

using namespace std;
using namespace glm;

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram              * shaderProgramDepth    = NULL;
    ShaderProgram              * shaderProgramFill     = NULL;
    ShaderProgram              * shaderProgramOutline  = NULL;
    ColorDistanceDepthShader3D * defaultInstance       = NULL;

    GLint vertInAttrDepth;
    GLint heightMultiplierDepth;
    GLint modelDepth;
    GLint viewDepth;
    GLint projectionDepth;

    GLint vertInAttrFill;
    GLint normalInAttrFill;
    GLint vertColorInAttrFill;

    GLint colorLookupOffsetFill;
    GLint heightMultiplierFill;
    GLint modelFill;
    GLint viewFill;
    GLint projectionFill;
	GLint invPersMatrixFill;
	GLint invViewMatrixFiLL;
	GLint MVP_Fill;
    GLint colorLookupTextureUniformFill;
	GLint topDownTextureUniformFill;
    GLint depthTexUniformFill;
    GLint lightPosUniformFill;
    GLint widthUniformFill;
    GLint heightUniformFill;
    
    GLint vertInAttrOutline;
    GLint vertColorInAttrOutline;
    GLint colorLookupOffsetOutline;
    GLint MV_Outline;
    GLint MVP_Outline;
    GLint texUniformOutline;
    GLint depthTexUniformOutline;
    GLint heightMultiplierOutline;
    GLint widthUniformOutline;
    GLint heightUniformOutline;
}

void ColorDistanceDepthShader3D::ensureShader()
{
	if(defaultInstance) { return ;}

	shaderProgramDepth = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_depth.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_depth.fs.glsl"		}}),
													Variables({	{"vertIn",					vertInAttrDepth									}}),
													Variables({	{"model",					modelDepth										},
																{"view",					viewDepth										},
																{"projection",				projectionDepth									},
																{"heightMultiplier",		heightMultiplierDepth							}}));

	shaderProgramFill = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_fill.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_fill.fs.glsl"		}}),
													Variables({	{"vertIn",					vertInAttrFill									},
																{"vertColorIn",				vertColorInAttrFill								},
																{"normalIn",				normalInAttrFill								}}),
													Variables({	{"colorLookupTexture",		colorLookupTextureUniformFill					},
																{"topDownTexture",			topDownTextureUniformFill						},
																{"depthTex",				depthTexUniformFill								},
																{"model",					modelFill										},
																{"view",					viewFill										},
																{"projection",				projectionFill									},
																{"invPersMatrix",			invPersMatrixFill								},
																{"invViewMatrix",			invViewMatrixFiLL								},
														{"MVP", MVP_Fill},
																{"colorLookupOffset",		colorLookupOffsetFill							},
																{"heightMultiplier",		heightMultiplierFill							},
																{"lightPos",				lightPosUniformFill								},
																{"width",					widthUniformFill								},
																{"height",					heightUniformFill								}}));

	shaderProgramOutline = ShaderProgram::create(   GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_outline.vs.glsl"	},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_outline.fs.glsl"	}}),
                                                    Variables({	{"vertIn",					vertInAttrOutline								},
																{"vertColorIn",				vertColorInAttrOutline							}}),
                                                    Variables({	{"MV",						MV_Outline										},
																{"MVP",						MVP_Outline										},
																{"tex",						texUniformOutline								},
																{"depthTex",				depthTexUniformOutline							},
																{"colorLookupOffset",		colorLookupOffsetOutline						},
																{"heightMultiplier",		heightMultiplierOutline							},
																{"width",					widthUniformOutline								},
																{"height",					heightUniformOutline							}
																}));

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
            dmess("Error!");
            abort();
    }
}

void ColorDistanceDepthShader3D::bindStage1(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage1");

    shaderProgramDepth->bind();

    GL_CHECK(glEnable(GL_DEPTH_TEST));

    shaderProgramDepth->setUniformf(heightMultiplierDepth,    m_heightMultiplier);
    shaderProgramDepth->setUniform (modelDepth,               canvas->getModelRef());
    shaderProgramDepth->setUniform (viewDepth,                canvas->getViewRef());
    shaderProgramDepth->setUniform (projectionDepth,          canvas->getProjectionRef());
}

GLuint theTex = 0;

void ColorDistanceDepthShader3D::bindStage0(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage0");

    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID()));

    GL_CHECK(glActiveTexture(GL_TEXTURE1));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, canvas->getAuxFrameBuffer()->getTextureID()));

	GL_CHECK(glActiveTexture(GL_TEXTURE2));

	GL_CHECK(glBindTexture(GL_TEXTURE_2D, theTex));

    GL_CHECK(glEnable(GL_BLEND));

    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glDisable(GL_DEPTH_TEST));

    if(!isOutline)
    {
        shaderProgramFill->bind();
        
        shaderProgramFill->setUniformi(colorLookupTextureUniformFill,   0);
        shaderProgramFill->setUniformi(depthTexUniformFill,				1);
		shaderProgramFill->setUniformi(topDownTextureUniformFill,		2);
        shaderProgramFill->setUniformf(widthUniformFill,				(float)canvas->getFrameBufferSize().x);
        shaderProgramFill->setUniformf(heightUniformFill,				(float)canvas->getFrameBufferSize().y);
        shaderProgramFill->setUniformf(heightMultiplierFill,			m_heightMultiplier);
        shaderProgramFill->setUniform (modelFill,						canvas->getModelRef());
        shaderProgramFill->setUniform (viewFill,						canvas->getViewRef());
        shaderProgramFill->setUniform (projectionFill,					canvas->getProjectionRef());

		shaderProgramFill->setUniform(invPersMatrixFill,  inverse(canvas->getProjectionRef()));
		shaderProgramFill->setUniform(invViewMatrixFiLL,	inverse(canvas->getViewRef()));
		shaderProgramFill->setUniform(MVP_Fill, canvas->getMVP_Ref());

        shaderProgramFill->setUniform (lightPosUniformFill,				m_lightPos);
        shaderProgramFill->setUniformf(colorLookupOffsetFill,			0.0f);
    }
    else
    {
        shaderProgramOutline->bind();
        
        shaderProgramOutline->setUniformf(heightMultiplierOutline,  m_heightMultiplier);
        shaderProgramOutline->setUniformi(depthTexUniformOutline,   1);
        shaderProgramOutline->setUniformf(widthUniformOutline,      (float)canvas->getFrameBufferSize().x);
        shaderProgramOutline->setUniformf(heightUniformOutline,     (float)canvas->getFrameBufferSize().y);
        shaderProgramOutline->setUniform (MV_Outline,               canvas->getMV_Ref());
        shaderProgramOutline->setUniform (MVP_Outline,              canvas->getMVP_Ref());
        shaderProgramOutline->setUniformi(texUniformOutline,        0);
        shaderProgramOutline->setUniformf(colorLookupOffsetOutline, 1.0f);
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

vec3 ColorDistanceDepthShader3D::setLightPos(const vec3 & pos)	{ return m_lightPos = pos ;}
vec3 ColorDistanceDepthShader3D::getLightPos() const			{ return m_lightPos		  ;}

size_t ColorDistanceDepthShader3D::getNumRenderingStages() const { return 2 ;}
