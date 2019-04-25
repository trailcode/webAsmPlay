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
#include <glm/gtx/transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/renderables/SkyBox.h>

using namespace std;
using namespace glm;

#ifdef __EMSCRIPTEN__
    using namespace emscripten;
#endif

namespace
{
    ShaderProgram * skyboxShader = NULL;

    GLint vertInLoc      = -1;
    GLint MVP_Loc        = -1;
    GLint cubeTextureLoc = -1;

    vector<SkyBox *> instances;
}

void SkyBox::ensureShader()
{
    dmess("SkyBox::ensureShader");
    
    if(skyboxShader) { return ;}

	skyboxShader = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"SkyBoxShader.vs.glsl"	},
													{GL_FRAGMENT_SHADER,	"SkyBoxShader.fs.glsl"	}}),
                                         Variables({{"vertIn",				vertInLoc				}}),
                                         Variables({{"MVP",					MVP_Loc					},
                                                    {"cubeTexture",			cubeTextureLoc			}}));
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
    

    m_texID = Textures::loadCube(files);

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

    GL_CHECK(glGenBuffers(1, &m_vbo));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));
    GL_CHECK(glGenVertexArrays(1, &m_vao));
    GL_CHECK(glBindVertexArray(m_vao));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL));

    m_model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
}

SkyBox::~SkyBox()
{
    // TODO cleanup! Remove from instances
}

void SkyBox::render(Canvas * canvas)
{
    mat4 centeredView = mat4(canvas->getViewRef());

    value_ptr(centeredView)[12] = 0;
    value_ptr(centeredView)[13] = 0;
    value_ptr(centeredView)[14] = 0;

    GL_CHECK(glBindVertexArray(m_vao));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    
    skyboxShader->bind();

    skyboxShader->setUniform (MVP_Loc,        mat4(canvas->getProjectionRef()) * centeredView * m_model);
    skyboxShader->setUniformi(cubeTextureLoc, 0);

    GL_CHECK(glDepthMask(GL_FALSE));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
    GL_CHECK(glDepthMask(GL_TRUE));
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
