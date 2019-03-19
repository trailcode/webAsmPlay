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

#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>

namespace
{
	ShaderProgram			* shaderProgram   = NULL;
	BindlessTextureShader	* defaultInstance = NULL;

	GLint vertInAttrLoc;
	GLint vertUV_InAttrLoc;

	GLint MVP_Loc;
	GLint texLoc;
	GLint texID_Loc;
}

BindlessTextureShader* BindlessTextureShader::getDefaultInstance() { return defaultInstance ;}

void BindlessTextureShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  "TextureShader.vs.glsl",
											"TextureShader.fs.glsl",
											Variables({{"vertIn",       vertInAttrLoc},
													   {"vertUV_In",    vertUV_InAttrLoc}
													  }),
											Variables({{"MVP",          MVP_Loc},
													   {"tex",          texLoc},
													   {"texID",		texID_Loc}}));

	defaultInstance = new BindlessTextureShader();
}

void BindlessTextureShader::bind(	Canvas		* canvas,
									const bool    isOutline,
									const size_t  renderingStage)
{
	shaderProgram->bind();

	shaderProgram->setUniformi(texID_Loc, textureSlot);

	shaderProgram->setUniform(MVP_Loc, canvas->getMVP_Ref());
}

BindlessTextureShader::BindlessTextureShader() : Shader("BindlessTextureShader")
{

}

BindlessTextureShader::~BindlessTextureShader()
{

}

GLuint BindlessTextureShader::setTextureSlot(const GLuint textureSlot)
{
	return this->textureSlot = textureSlot;
}