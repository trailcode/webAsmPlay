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
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>

using namespace std;
using namespace glm;

//REGISTER_SHADER(ColorDistanceDepthShader3D)

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram              * shaderProgramPre    = NULL;
    ShaderProgram              * shaderProgramFill     = NULL;
    ShaderProgram              * shaderProgramOutline  = NULL;
    ColorDistanceDepthShader3D * defaultInstance       = NULL;

    GLint vertInAttrPre;
    GLint heightMultiplierPre;
    GLint modelPre;
    GLint viewPre;
    GLint projectionPre;

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

	shaderProgramPre = ShaderProgram::create(		GLSL({		{GL_VERTEX_SHADER,			"ColorDistanceDepthShader3D_pre.vs.glsl"		},
																{GL_FRAGMENT_SHADER,		"ColorDistanceDepthShader3D_pre.fs.glsl"		}}),
													Variables({	{"vertIn",					vertInAttrPre									}}),
													Variables({	{"model",					modelPre										},
																{"view",					viewPre											},
																{"projection",				projectionPre									},
																{"heightMultiplier",		heightMultiplierPre								}}));

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
																{"MVP",						MVP_Fill										},
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
            dmessError("Error!");
    }
}

void ColorDistanceDepthShader3D::bindStage1(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage1");

    shaderProgramPre->bind();

	glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    shaderProgramPre->setUniformf(heightMultiplierPre,    m_heightMultiplier);
    shaderProgramPre->setUniform (modelPre,               canvas->getModelRef());
    shaderProgramPre->setUniform (viewPre,                canvas->getViewRef());
    shaderProgramPre->setUniform (projectionPre,          canvas->getProjectionRef());
}

void ColorDistanceDepthShader3D::bindStage0(Canvas * canvas, const bool isOutline)
{
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_colorSymbology->getTextureID());

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, canvas->getAuxFrameBuffer()->getTextureID());

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
        shaderProgramFill->setUniformf(widthUniformFill,				(float)canvas->getFrameBufferSize().x);
        shaderProgramFill->setUniformf(heightUniformFill,				(float)canvas->getFrameBufferSize().y);
        shaderProgramFill->setUniformf(heightMultiplierFill,			m_heightMultiplier);
        shaderProgramFill->setUniform (modelFill,						canvas->getModelRef());
        shaderProgramFill->setUniform (viewFill,						canvas->getViewRef());
        shaderProgramFill->setUniform (projectionFill,					canvas->getProjectionRef());
		shaderProgramFill->setUniform(invPersMatrixFill,				inverse(canvas->getProjectionRef()));
		shaderProgramFill->setUniform(invViewMatrixFiLL,				inverse(canvas->getViewRef()));
		shaderProgramFill->setUniform(MVP_Fill,							canvas->getMVP_Ref());
        shaderProgramFill->setUniform (lightPosUniformFill,				m_lightPos);
        shaderProgramFill->setUniformf(colorLookupOffsetFill,			0.0f);
    }
    else
    {
        shaderProgramOutline->bind();
        
        shaderProgramOutline->setUniformf(heightMultiplierOutline,		m_heightMultiplier);
        shaderProgramOutline->setUniformi(depthTexUniformOutline,		1);
        shaderProgramOutline->setUniformf(widthUniformOutline,			(float)canvas->getFrameBufferSize().x);
        shaderProgramOutline->setUniformf(heightUniformOutline,			(float)canvas->getFrameBufferSize().y);
        shaderProgramOutline->setUniform (MV_Outline,					canvas->getMV_Ref());
        shaderProgramOutline->setUniform (MVP_Outline,					canvas->getMVP_Ref());
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
