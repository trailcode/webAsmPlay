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
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/TextureShader.h>

namespace
{
    ShaderProgram * shaderProgram   = NULL;
    TextureShader * defaultInstance = NULL;

    GLint vertInAttrLoc;
    GLint vertUV_InAttrLoc;

    GLint MVP_Loc;
    GLint texLoc;
}

TextureShader * TextureShader::getDefaultInstance() { return defaultInstance ;}

void TextureShader::ensureShader()
{
    if(shaderProgram) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec3 vertIn;
        in vec2 vertUV_In;
        out vec2 UV;
        uniform mat4 MVP;
        
        void main()
        {
            gl_Position = MVP * vec4(vertIn.xyz, 1);
            UV = vertUV_In;
            //UV = vec2(0,0);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec2 UV;
        uniform sampler2D tex;

        void main()
        {
            outColor = texture( tex, UV );
            //outColor = vec4(0,1,1,0.5);
            outColor.a = 1.0;
        }
    )glsl";

    shaderProgram = ShaderProgram::create(  vertexSource,
                                            fragmentSource,
                                            Variables({{"vertIn",       vertInAttrLoc},
                                                       {"vertUV_In",    vertUV_InAttrLoc}
                                                       }),
                                            Variables({{"MVP",          MVP_Loc},
                                                       {"tex",          texLoc}}));

    dmess("shaderProgram " << shaderProgram);

    defaultInstance = new TextureShader();
}

TextureShader::TextureShader() : Shader("TextureShader")
{

}

TextureShader::~TextureShader()
{

}

GLuint TextureShader::setTextureID(const GLuint textureID) { return this->textureID = textureID ;}

void TextureShader::bind(   Canvas     * canvas,
                            const bool   isOutline,
                            const size_t renderingStage)
{
    shaderProgram->bind();

    ShaderProgram::enableVertexAttribArray( vertInAttrLoc,
                                            vertexFormat.size,
                                            GL_FLOAT,
                                            GL_FALSE,
                                            vertexFormat.stride,
                                            vertexFormat.pointer);

    ShaderProgram::enableVertexAttribArray( vertUV_InAttrLoc,
                                            uvFormat.size,
                                            GL_FLOAT,
                                            GL_FALSE,
                                            uvFormat.stride,
                                            uvFormat.pointer);

    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));

    shaderProgram->setUniformi(texLoc, 0);

    shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());
}
