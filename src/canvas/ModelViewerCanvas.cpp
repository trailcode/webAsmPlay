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
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/shaders/PhongShader.h>
#include <webAsmPlay/renderables/Model.h>
#include <webAsmPlay/canvas/ModelViewerCanvas.h>

using namespace glm;

namespace
{
	Model * a_model = nullptr;

	mat4 * modelMatrices = nullptr;

	size_t amount = 10000;
}

ModelViewerCanvas::ModelViewerCanvas(	const bool   useFrameBuffer,
										const vec4 & clearColor) : Canvas(true, clearColor)
{
	a_model = new Model("C:/build/LearnOpenGL/resources/objects/cartoon_lowpoly_trees_blend.obj");

    modelMatrices = new mat4[amount];

	float offset = 3.0f;

	for(size_t i = 0; i < amount; ++i)
	{
		auto model = mat4(1.0f);

		float x = (rand() % (int)(2 * offset * 10000)) / 10000.0f - offset;
		float y = (rand() % (int)(2 * offset * 10000)) / 10000.0f - offset;

		model = translate(model, glm::vec3(x, y, 0));

		model = scale(model, vec3(0.001, 0.001, 0.001));

		model = rotate(model, radians(90.0f), vec3(1,0,0));

		modelMatrices[i] = model;
	}
}

ModelViewerCanvas::~ModelViewerCanvas()
{
}

GLuint ModelViewerCanvas::render()
{
	preRender();

	PhongShader::getDefaultInstance()->bind(this, false);

	for(size_t i = 0; i < amount; ++i)
	{
		PhongShader::getDefaultInstance()->setModel(modelMatrices[i]);

		a_model->Draw([](const Material & material)
		{
			PhongShader::getDefaultInstance()->setMaterial(material);
		});
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return m_frameBuffer->getTextureID();
}
