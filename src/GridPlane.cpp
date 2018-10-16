#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
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

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

    glDepthMask(GL_FALSE);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glFlush();
}

#endif // WORKING