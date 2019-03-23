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
	GLint texID_Loc;
}

TextureShader * TextureShader::getDefaultInstance() { return defaultInstance ;}

void TextureShader::ensureShader()
{
    if(shaderProgram) { return ;}

    shaderProgram = ShaderProgram::create(  "TextureShader.vs.glsl",
                                            "TextureShader.fs.glsl",
                                            Variables({{"vertIn",       vertInAttrLoc},
                                                       {"vertUV_In",    vertUV_InAttrLoc}
                                                       }),
                                            Variables({{"MVP",          MVP_Loc},
                                                       {"tex",          texLoc}}));

    defaultInstance = new TextureShader();
}

TextureShader::TextureShader() : Shader("TextureShader")
{

}

TextureShader::~TextureShader()
{

}

GLuint TextureShader::setTextureID(const GLuint textureID)
{
	//shaderProgram->setUniformi(texID_Loc, textureID);

	return this->textureID = textureID;
}

GLuint TextureShader::setTextureID2(const GLuint textureID2)
{
	return this->textureID2 = textureID2;
}

GLuint64 TextureShader::setTextureHandle(const GLuint64 & handle) { return this->handle = handle ;}

void TextureShader::bind(   Canvas     * canvas,
                            const bool   isOutline,
                            const size_t renderingStage)
{
    shaderProgram->bind();

    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));

	//dmess("texID_Loc " << texID_Loc);

	//glProgramUniformHandleui64ARB(shaderProgram->getProgramHandle(), texID_Loc, handle);
	//glUniformHandleui64ARB(texID_Loc, handle);

    shaderProgram->setUniformi(texLoc, 0);
	//shaderProgram->setUniformi(texID_Loc, textureID2);

    shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());
}
