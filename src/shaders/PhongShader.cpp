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

#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/renderables/Mesh.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/PhongShader.h>

using namespace glm;

namespace
{
	ShaderProgram   * a_shaderProgram   = nullptr;
    PhongShader		* a_defaultInstance = nullptr;

	GLint a_pos					= -1;
	GLint a_normal				= -1;
	GLint a_model				= -1;
	GLint a_view				= -1;
	GLint a_projection			= -1;
	GLint a_viewPos				= -1;
	GLint a_materialAmbient		= -1;
	GLint a_materialDiffuse		= -1;
	GLint a_materialSpecular	= -1;
	GLint a_materialShininess	= -1;
	GLint a_lightPosition		= -1;
	GLint a_lightAmbient		= -1;
	GLint a_lightDiffuse		= -1;
	GLint a_lightSpecular		= -1;
}

void PhongShader::ensureShader()
{
#ifndef __EMSCRIPTEN__
	if(a_shaderProgram) { return ;}
	
	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"PhongShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"PhongShader.fs.glsl"	}}),
                                            Variables({	{"aPos",				a_pos					},
														{"aNormal",				a_normal				}}),
                                            Variables({ {"view",				a_view					},
														{"projection",			a_projection			},
														{"viewPos",				a_viewPos				},
														{"model",				a_model					},
														{"material.ambient",	a_materialAmbient		},
														{"material.diffuse",	a_materialDiffuse		},
														{"material.specular",	a_materialSpecular		},
														{"material.shininess",	a_materialShininess		},
														{"light.position",		a_lightPosition			},
														{"light.ambient",		a_lightAmbient			},
														{"light.diffuse",		a_lightDiffuse			},
														{"light.specular",		a_lightSpecular			}}));

    a_defaultInstance = new PhongShader();
	#else
	dmess("Fix!");
	#endif
}

PhongShader * PhongShader::getDefaultInstance() { return a_defaultInstance ;}

PhongShader::PhongShader() : Shader("ColorVertexShader",
									ColorSymbology::getInstance("defaultPolygon"),
									Shader::s_defaultShouldRender) {}

PhongShader::~PhongShader()
{

}

void PhongShader::bind( Canvas     * canvas,
						const bool   isOutline,
						const size_t renderingStage)
{
#ifndef __EMSCRIPTEN__
	a_shaderProgram->bind();

	glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down

	a_shaderProgram->setUniform(a_model,		model);
	a_shaderProgram->setUniform(a_view,			canvas->getViewRef());
	a_shaderProgram->setUniform(a_projection,	canvas->getProjectionRef());
	a_shaderProgram->setUniform(a_viewPos,		canvas->getCamera()->getCenterConstRef());

	// lighting
	glm::vec3 lightPos(1.2f, 1.0f, -2.0f); 

	a_shaderProgram->setUniform(a_lightPosition, lightPos);

	a_shaderProgram->setUniform(a_lightAmbient,		vec3(1.0f, 1.0f, 1.0f));
	a_shaderProgram->setUniform(a_lightDiffuse,		vec3(1.0f, 1.0f, 1.0f));
	a_shaderProgram->setUniform(a_lightSpecular,	vec3(1.0f, 1.0f, 1.0f));

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	#else
	dmess("Fix!");
	#endif
}

void PhongShader::setMaterial(const Material & mat) const
{
	a_shaderProgram->setUniform (a_materialAmbient,   mat.Ambient);
	a_shaderProgram->setUniform (a_materialDiffuse,   mat.Diffuse);
	a_shaderProgram->setUniform (a_materialSpecular,  mat.Specular);
	a_shaderProgram->setUniformf(a_materialShininess, mat.Shininess);	
}

void PhongShader::setModel(const mat4 & model) const
{
	a_shaderProgram->setUniform(a_model, model);
}