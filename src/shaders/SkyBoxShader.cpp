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
\copyright 2019
*/

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/SkyBoxShader.h>

using namespace glm;

REGISTER_SHADER(SkyBoxShader)

namespace
{
	ShaderProgram   * shaderProgram   = NULL;
	SkyBoxShader	* defaultInstance = NULL;

	GLint vertInLoc      = -1;
	GLint MVP_Loc        = -1;
	GLint cubeTextureLoc = -1;

	const mat4 model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
}

void SkyBoxShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({	{GL_VERTEX_SHADER,		"SkyBoxShader.vs.glsl"		},
													{GL_FRAGMENT_SHADER,	"SkyBoxShader.fs.glsl"		}}),
											Variables({	{"vertIn",			vertInLoc					}}),
											Variables({	{"MVP",				MVP_Loc						},
														{"cubeTexture",		cubeTextureLoc				}}));

	defaultInstance = new SkyBoxShader();
}

SkyBoxShader::SkyBoxShader() : Shader("SkyBoxShader") {}
SkyBoxShader::~SkyBoxShader() {}

SkyBoxShader * SkyBoxShader::getDefaultInstance() { return defaultInstance ;}

void SkyBoxShader::bind(Canvas     * canvas,
						const bool   isOutline,
						const size_t renderingStage)
{
	shaderProgram->bind();

	mat4 centeredView = mat4(canvas->getViewRef());

	value_ptr(centeredView)[12] = 0;
	value_ptr(centeredView)[13] = 0;
	value_ptr(centeredView)[14] = 0;

	shaderProgram->setUniform (MVP_Loc,        mat4(canvas->getProjectionRef()) * centeredView * model);
	shaderProgram->setUniformi(cubeTextureLoc, 0);
}
