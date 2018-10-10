#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/SkyBox.h>

using namespace std;
using namespace glm;

namespace
{
    Shader * skyboxShader = NULL;
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
            //gl_Position = P * V * M * vec4(position, 1.0);
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
            //frag_colour = vec4(1,0,0,1);
        }
    )glsl";

    skyboxShader = Shader::create(vertexSource, fragmentSource);
}

SkyBox::SkyBox()
{
    ensureShader();

    vector<string> files;

    files.push_back("xpos.png");
    files.push_back("xneg.png");
    files.push_back("ypos.png");
    files.push_back("yneg.png");
    files.push_back("zpos.png");
    files.push_back("zneg.png");

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

    dmess("vbo " << vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    dmess("vao " << vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //model = rotate(radians(90), vec3(1,0,0))
    model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
}

SkyBox::~SkyBox()
{

}

void SkyBox::render(const mat4 & _view, const mat4 & projection)
{
    //dmess("render " << texID);

    skyboxShader->bind();

    mat4 centeredView = mat4(_view);

    value_ptr(centeredView)[12] = 0;
    value_ptr(centeredView)[13] = 0;
    value_ptr(centeredView)[14] = 0;

    const mat4 MVP = projection * centeredView * model;

    //dmess("centeredView " << mat4ToStr(centeredView));

    //glUniform1i(self.program.uniformLocation('cube_texture'), 1);
    skyboxShader->setTexture1Slot(1);

    skyboxShader->setMVP(MVP);

    //skyboxShader->enableVertexAttribArray(3);

    //*
    
    //glUniformMatrix4fv(self.program.uniformLocation('P'), 1, false, value_ptr(projection));
    //glUniformMatrix4fv(self.program.uniformLocation('V'), 1, false, centeredView);
    //glUniformMatrix4fv(self.program.uniformLocation('M'), 1, false, value_ptr(model));

    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glFlush();
    //*/
}


