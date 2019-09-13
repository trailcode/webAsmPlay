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
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/SkyBoxShader.h>

using namespace glm;

//REGISTER_SHADER(SkyBoxShader)

namespace
{
	ShaderProgram   * a_shaderProgram   = nullptr;
	SkyBoxShader	* a_defaultInstance = nullptr;

	GLint a_vertIn;
	GLint a_MVP;
	GLint a_cubeTexture;

	const mat4 model = rotate(radians(90.0f), vec3(1.0f,0.0f,0.0f));
}

void SkyBoxShader::ensureShader()
{
    dmess("SkyBoxShader::ensureShader");

	if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({	{GL_VERTEX_SHADER,		"SkyBoxShader.vs.glsl"		},
													{GL_FRAGMENT_SHADER,	"SkyBoxShader.fs.glsl"		}}),
											Variables({	{"vertIn",			a_vertIn					}}),
											Variables({	{"MVP",				a_MVP						},
														{"cubeTexture",		a_cubeTexture				}}));

	a_defaultInstance = new SkyBoxShader();
}

SkyBoxShader::SkyBoxShader() : Shader(	"SkyBoxShader",
										nullptr,
										Shader::s_defaultShouldRender) {}

SkyBoxShader::~SkyBoxShader() {}

SkyBoxShader * SkyBoxShader::getDefaultInstance() { return a_defaultInstance ;}

void SkyBoxShader::bind(Canvas     * canvas,
						const bool   isOutline,
						const size_t renderingStage)
{
	a_shaderProgram->bind();
	
	glDisable(GL_DEPTH_TEST);

	mat4 centeredView = mat4(canvas->getViewRef());

	value_ptr(centeredView)[12] = 0;
	value_ptr(centeredView)[13] = 0;
	value_ptr(centeredView)[14] = 0;

	a_shaderProgram->setUniform (a_MVP,         mat4(canvas->getProjectionRef()) * centeredView * model);
	a_shaderProgram->setUniformi(a_cubeTexture, 0);
}
