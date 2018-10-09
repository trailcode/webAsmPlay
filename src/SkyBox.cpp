#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/SkyBox.h>

namespace
{
    Shader * skyboxShader = NULL;
}

void SkyBox::ensureShader()
{
    if(skyboxShader) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 vp;
        uniform mat4 M, P, V; 
        out vec3 texcoords;
        
        void main()
        {
            texcoords = vp;
            gl_Position = P * V * M * vec4(vp, 1.0);
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

    skyboxShader = Shader::create(vertexSource, fragmentSource);
}

SkyBox::SkyBox() :  xpos(Textures::load("xpos.png")),
                    xneg(Textures::load("xneg.png")),
                    ypos(Textures::load("ypos.png")),
                    yneg(Textures::load("yneg.png")),
                    zpos(Textures::load("zpos.png")),
                    zneg(Textures::load("zneg.png"))
{
    ensureShader();

    dmess("xpos " << xpos);
    dmess("xneg " << xneg);
    dmess("ypos " << ypos);
    dmess("yneg " << yneg);
    dmess("zpos " << zpos);
    dmess("zneg " << zneg);

}

SkyBox::~SkyBox()
{

}

void SkyBox::render()
{

}


