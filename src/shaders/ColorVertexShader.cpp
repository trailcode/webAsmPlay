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
#include <webAsmPlay/shaders/ColorVertexShader.h>

using namespace glm;

namespace
{
    ShaderProgram     * shaderProgram   = NULL;
    ColorVertexShader * defaultInstance = NULL;

    GLint vertInAttrLoc;
    GLint vertColorInAttrLoc;

    GLint MVP_Loc;
}

void ColorVertexShader::ensureShader()
{
    if(shaderProgram) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 vertIn;
        in vec4 vertColorIn;
        out vec4 vertexColor;
        uniform mat4 MVP;
        
        void main()
        {
            gl_Position = MVP * vec4(vertIn.xyz, 1);
            vertexColor = vertColorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vertexColor;
        }
    )glsl";

    shaderProgram = ShaderProgram::create(  vertexSource,
                                            fragmentSource,
                                            Variables({{"vertIn",      vertInAttrLoc},
                                                       {"vertColorIn", vertColorInAttrLoc}}),
                                            Variables({{"MVP",         MVP_Loc}}));

    defaultInstance = new ColorVertexShader();
}

ColorVertexShader::ColorVertexShader() : Shader("ColorVertexShader",
                                                shaderProgram,
                                                vertInAttrLoc,
                                                vertColorInAttrLoc) {}
ColorVertexShader::~ColorVertexShader() {}

ColorVertexShader * ColorVertexShader::getInstance() { return defaultInstance ;}

void ColorVertexShader::bind(Canvas * canvas, const bool isOutline)
{
    shaderProgram->bind();

    shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());
}
