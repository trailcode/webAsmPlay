
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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/ColorDistanceShader.h>

using namespace glm;

namespace
{
    Shader * instance = NULL;

    GLint MV_Uniform = -1;
}

void ColorDistanceShader::ensureShader()
{
    dmess("ColorDistanceShader::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 vertIn;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform mat4 MV;
        uniform vec4 colorIn;

        out vec4 position_in_view_space;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, 0, 1);

            position_in_view_space = MV * vert;

            gl_Position = MVP * vert;

            vertexColor = colorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        in vec4 position_in_view_space;

        void main()
        {
            //outColor = vertexColor;

            // computes the distance between the fragment position 
            // and the origin (4th coordinate should always be 1 
            // for points). The origin in view space is actually 
            // the camera position.
            float dist = distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0));
            
            float farDist = 10.0f;

            dist = min(farDist, dist) / farDist;

            outColor = vec4(1.0, 0, 0, 1) * (1 - dist) + vec4(0,1.0,0,1) * dist;
        }
    )glsl";

    instance = Shader::create(vertexSource, fragmentSource, StrVec({"MV"}));

    MV_Uniform = instance->getUniformLoc("MV");

    dmess("MV_Uniform " << MV_Uniform);
}

void ColorDistanceShader::bind(const mat4 & MVP, const mat4 & MV)
{
    instance->Shader::bind(MVP, MV);

    instance->setUniform(MV_Uniform, MV);
}


