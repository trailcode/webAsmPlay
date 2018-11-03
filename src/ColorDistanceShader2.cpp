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

#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/ColorDistanceShader2.h>

using namespace glm;

namespace
{
    Shader * instance = NULL;

    GLint colorsInLoc = -1;

    glm::vec4 colors[32];
}

void ColorDistanceShader2::ensureShader()
{
    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 vertIn;
        in int vertColorIn;
        out vec4 vertexColor;

        uniform mat4 MVP;
        uniform vec4 colorsIn[32];

        void main()
        {
            gl_Position = MVP * vec4(vertIn.xy, 0, 1);
            vertexColor = colorsIn[vertColorIn * 2];
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

    instance = Shader::create(vertexSource, fragmentSource);

    colorsInLoc = instance->getUniformLoc("colorsIn");

    colors[0] = vec4(1,0,0,1);
    colors[1] = vec4(1,1,0,1);
    colors[2] = vec4(1,0,1,1);
    colors[3] = vec4(0,1,0,1);
    colors[4] = vec4(0,1,1,1);
    colors[5] = vec4(0,0,1,1);
    colors[6] = vec4(1,0,0,1);
    colors[7] = vec4(1,0,0,1);

    glUniform4fv(colorsInLoc, 32, (const GLfloat *)colors);
}

void ColorDistanceShader2::bind(const mat4 & MVP, const mat4 & MV)
{
    instance->bind(MVP, MV);

    instance->enableVertexAttribArray(2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    instance->enableColorAttribArray(1, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(2 * sizeof(GLuint)));

    glUniform4fv(colorsInLoc, 32, (const GLfloat *)colors);
}

vec4 ColorDistanceShader2::setColor(const size_t index, const vec4 & color)
{
    return colors[index] = color;
}

vec4 ColorDistanceShader2::getColor(const size_t index)
{
    return colors[index];
}
