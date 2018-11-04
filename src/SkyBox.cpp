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

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#endif
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/SkyBox.h>

using namespace std;
using namespace glm;

#ifdef __EMSCRIPTEN__
    using namespace emscripten;
#endif

namespace
{
    ShaderProgram * skyboxShader = NULL;

    vector<SkyBox *> instances;
}

void SkyBox::ensureShader()
{
    if(skyboxShader) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 position;
        uniform mat4 MVP; 
        out vec3 texcoords;
        
        void main()
        {
            texcoords = position;

            gl_Position = MVP * vec4(position, 1.0);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        in vec3 texcoords;
        uniform samplerCube cube_texture;
        out vec4 frag_colour;
        
        void main()
        {
            frag_colour = texture(cube_texture, texcoords);
        }
    )glsl";

    skyboxShader = ShaderProgram::create(vertexSource, fragmentSource);
}

SkyBox::SkyBox()
{
    instances.push_back(this);

    ensureShader();

    vector<string> files;

    /*
    files.push_back("xpos.png");
    files.push_back("xneg.png");
    files.push_back("ypos.png");
    files.push_back("yneg.png");
    files.push_back("zpos.png");
    files.push_back("zneg.png");
    */
 
    files.push_back("right.png"); // xpos
    files.push_back("left.png");  // xneg
    files.push_back("top.png");   // ypos
    files.push_back("bot.png");   // yneg
    files.push_back("front.png"); // zpos
    files.push_back("back.png");  // zneg
    

    texID = Textures::loadCube(files);

    const GLfloat verts[] = {   -10.0,  10.0, -10.0,
                                -10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                 10.0,  10.0, -10.0,
                                -10.0,  10.0, -10.0,

                                -10.0, -10.0,  10.0,
                                -10.0, -10.0, -10.0,
                                -10.0,  10.0, -10.0,
                                -10.0,  10.0, -10.0,
                                -10.0,  10.0,  10.0,
                                -10.0, -10.0,  10.0,

                                 10.0, -10.0, -10.0,
                                 10.0, -10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0, -10.0,
                                 10.0, -10.0, -10.0,

                                -10.0, -10.0,  10.0,
                                -10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                 10.0, -10.0,  10.0,
                                -10.0, -10.0,  10.0,

                                -10.0,  10.0, -10.0,
                                 10.0,  10.0, -10.0,
                                 10.0,  10.0,  10.0,
                                 10.0,  10.0,  10.0,
                                -10.0,  10.0,  10.0,
                                -10.0,  10.0, -10.0,

                                -10.0, -10.0, -10.0,
                                -10.0, -10.0,  10.0,
                                 10.0, -10.0, -10.0,
                                 10.0, -10.0, -10.0,
                                -10.0, -10.0,  10.0,
                                 10.0, -10.0,  10.0 };

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
}

SkyBox::~SkyBox()
{
    // TODO cleanup! Remove from instances
}

void SkyBox::render(const mat4 & _view, const mat4 & projection)
{
    mat4 centeredView = mat4(_view);

    value_ptr(centeredView)[12] = 0;
    value_ptr(centeredView)[13] = 0;
    value_ptr(centeredView)[14] = 0;

    const mat4 MVP = projection * centeredView * model;

    skyboxShader->bind(MVP, mat4(1.0));

    skyboxShader->setTexture1Slot(1);

    //skyboxShader->enableVertexAttribArray(3); // TODO Why is this not required here?

    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glFlush();
}

vector<SkyBox *> SkyBox::getInstances() { return instances ;}

#ifdef __EMSCRIPTEN__

EMSCRIPTEN_BINDINGS(SkyBoxBindings)
{
    register_vector<SkyBox *>("VectorSkyBoxPtr");
    class_<SkyBox>("SkyBox")
    .class_function("getInstances", &SkyBox::getInstances)
    ;
}

#endif
