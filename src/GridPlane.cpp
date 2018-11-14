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

#include <glm/gtx/transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/GridPlane.h>

#ifdef WORKING

using namespace std;
using namespace glm;

namespace
{
    Shader * shader = NULL;
}

void GridPlane::ensureShader()
{
    if(shader) { return ;}

    dmess("GridPlane::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 position;
        uniform mat4 MVP;
        void main()
        {
            gl_Position = MVP * vec4(position.xyz, 1);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        //uniform vec2 u_resolution;

        void main(){
            float vpw = 800.0f;
            float vph = 600.0f;
            float lX = gl_FragCoord.x / vpw;
            float lY = gl_FragCoord.y / vph;
            vec2 offset = vec2(-0.023500000000000434,0.9794000000000017);
            vec2 pitch = vec2(50,50);

            float scaleFactor = 10000.0;

            float offX = (scaleFactor * offset[0]) + gl_FragCoord.x;
            float offY = (scaleFactor * offset[1]) + (1.0 - gl_FragCoord.y);

            if (int(mod(offX, pitch[0])) == 0 ||
                int(mod(offY, pitch[1])) == 0) {
                outColor = vec4(0.0, 0.0, 0.0, 0.5);
            } else {
                outColor = vec4(1.0, 1.0, 1.0, 1.0);
            }
        }
    )glsl";

    shader = Shader::create(vertexSource, fragmentSource);
}

GridPlane::GridPlane()
{
    const GLfloat verts[] = {  
                                ///*
                                 -10.0,  10.0, -10.0,
                                -10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                 10.0,  10.0, -10.0,
                                -10.0,  10.0, -10.0,
                                //*/


                                -10.0, -10.0,  10.0,
                                -10.0, -10.0, -10.0,
                                -10.0,  10.0, -10.0,
                                -10.0,  10.0, -10.0,
                                -10.0,  10.0,  10.0,
                                -10.0, -10.0,  10.0,

                                //*
                                 10.0, -10.0, -10.0,
                                 10.0, -10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                 //*/

                                //*
                                -10.0, -10.0,  10.0,
                                -10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0, -10.0,  10.0,
                                -10.0, -10.0,  10.0,
                                //*/

                                //*
                                -10.0,  10.0, -10.0,
                                 10.0,  10.0, -10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                -10.0,  10.0,  10.0,
                                -10.0,  10.0, -10.0,
                                //*/

                                //*
                                -10.0, -10.0, -10.0,
                                -10.0, -10.0,  10.0,
                                 10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                -10.0, -10.0,  10.0,
                                 10.0, -10.0,  10.0 
                                 //*/
                                 };

    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));

    //model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
    model = rotate(radians(0.0f), vec3(1.0f,0.0f,0.0f));
}

GridPlane::~GridPlane()
{

}

void GridPlane::render(const mat4 & MVP) const
{
    return;
    
    shader->bind();

    shader->setMVP(MVP);

    GL_CHECK(glDepthMask(GL_FALSE));
    GL_CHECK(glBindVertexArray(vao));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
    GL_CHECK(glDepthMask(GL_TRUE));
    GL_CHECK(glFlush());
}

#endif // WORKING