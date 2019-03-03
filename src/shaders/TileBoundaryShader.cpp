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
  \copyright 2019
*/

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/TileBoundaryShader.h>

namespace
{
    ShaderProgram * shaderProgram   = NULL;

    TileBoundaryShader * defaultInstance = NULL;

    GLint vertInAttrLoc;
    
    GLint MVP_Loc;
}

TileBoundaryShader * TileBoundaryShader::getDefaultInstance()
{
    return defaultInstance;
}

void TileBoundaryShader::ensureShader()
{
    if(shaderProgram) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 vertIn;
        uniform mat4 MVP;
        
        void main()
        {
            gl_Position = MVP * vec4(vertIn.xyz, 1);
        }
    )glsl";

    const GLchar * geometrySource = R"glsl(#version 330 core
        in vec3 vertIn;
        uniform mat4 MVP;
        
        void main()
        {
            gl_Position = MVP * vec4(vertIn.xyz, 1);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        uniform sampler2D tex;

        void main()
        {
            outColor = vec4(1,1,1,1);
        }
    )glsl";

    shaderProgram = ShaderProgram::create(  vertexSource,
                                            fragmentSource,
                                            geometrySource,
                                            Variables({{"vertIn",       vertInAttrLoc}
                                                       }),
                                            Variables({{"MVP",          MVP_Loc}}));

    defaultInstance = new TileBoundaryShader();
}

TileBoundaryShader::TileBoundaryShader() : Shader("TileBoundaryShader")
{

}

TileBoundaryShader::~TileBoundaryShader()
{

}

void TileBoundaryShader::bind(  Canvas     * canvas,
                                const bool   isOutline,
                                const size_t renderingStage)
{

}