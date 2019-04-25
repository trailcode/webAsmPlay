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
#include <webAsmPlay/shaders/ColorShader.h>

using namespace glm;

REGISTER_SHADER(ColorShader)

namespace
{
    ShaderProgram * shaderProgram   = NULL;
    ColorShader   * defaultInstance = NULL;

    GLint vertInAttrLoc;
    GLint colorInUniformLoc;

    GLint MVP_Loc;
}

void ColorShader::ensureShader()
{
    if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({		{GL_VERTEX_SHADER,		"ColorShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"ColorShader.fs.glsl"	}}),
                                            Variables({	{"vertIn",				vertInAttrLoc			}}),
                                            Variables({	{"MVP",					MVP_Loc					},
														{"colorIn",				colorInUniformLoc		}}));

    defaultInstance = new ColorShader();
}

ColorShader * ColorShader::getDefaultInstance() { return defaultInstance ;}

ColorShader::ColorShader() :    Shader			("ColorShader"),
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
    shaderProgram->bind();

    shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());

    if(isOutline) { shaderProgram->setUniform(colorInUniformLoc, m_outlineColor) ;}
    else          { shaderProgram->setUniform(colorInUniformLoc, m_fillColor)    ;}
}