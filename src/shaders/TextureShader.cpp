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
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/TextureShader.h>

//REGISTER_SHADER(TextureShader)

namespace
{
    ShaderProgram * a_shaderProgram   = nullptr;
    TextureShader * a_defaultInstance = nullptr;

    GLint a_vertInAttr;
    GLint a_vertUV_InAttr;

    GLint a_MVP;
    GLint a_tex;
}

TextureShader * TextureShader::getDefaultInstance() { return a_defaultInstance ;}

void TextureShader::ensureShader()
{
    if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"TextureShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"TextureShader.fs.glsl"	}}),
                                            Variables({	{"vertIn",				a_vertInAttr			},
														{"vertUV_In",			a_vertUV_InAttr			}}),
                                            Variables({	{"MVP",					a_MVP					},
														{"tex",					a_tex					}}));

    a_defaultInstance = new TextureShader([](const bool isOutline, const size_t renderingStage) -> bool
	{
		return renderingStage == G_BUFFER;
	});
}

TextureShader::TextureShader(const ShouldRenderFunctor & shouldRender) : Shader("TextureShader",
																				nullptr,
																				shouldRender)
{

}

TextureShader::~TextureShader()
{

}

TextureShader * TextureShader::setTextureID(const GLuint textureID)
{
	m_textureID = textureID;

	return this;
}

void TextureShader::bind(   Canvas     * canvas,
                            const bool   isOutline,
                            const size_t renderingStage)
{
    a_shaderProgram->bind();

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_textureID);

	a_shaderProgram->setUniformi(a_tex, 0);

    a_shaderProgram->setUniform(a_MVP, canvas->getMVP_Ref());
}
