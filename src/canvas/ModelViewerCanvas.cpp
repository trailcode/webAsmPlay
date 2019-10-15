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
#include <webAsmPlay/shaders/PhongShaderInstanced.h>
#include <webAsmPlay/renderables/Model.h>
#include <webAsmPlay/canvas/ModelViewerCanvas.h>

#include <thread>

using namespace std;
using namespace glm;

namespace
{
	Model * a_model = nullptr;

	mat4 * modelMatrices = nullptr;

	size_t amount = 10000;

	bool a_gotVAO = false;
}

ModelViewerCanvas::ModelViewerCanvas(	const bool   useFrameBuffer,
										const vec4 & clearColor) : Canvas(true, clearColor)
{
	thread worker([]()
	{
		OpenGL::ensureSharedContext();

		a_model = new Model("C:/build/LearnOpenGL/resources/objects/cartoon_lowpoly_trees_blend.obj");

		for (auto & mesh : a_model->meshes)
		{
			//mesh.ensureVAO();
		}

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
	});

	worker.join();
	/*
	for (auto & mesh : a_model->meshes)
    {
		//mesh.ensureVAO();

        unsigned int VAO = mesh.VAO;

        glBindVertexArray(VAO);
		//glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6); 
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
	*/
}

ModelViewerCanvas::~ModelViewerCanvas()
{
}

GLuint ModelViewerCanvas::render()
{
	if(!a_gotVAO)
	{
		a_gotVAO = true;

		for (auto & mesh : a_model->meshes)
		{
			mesh.ensureVAO();
		}

		// configure instanced array
		// -------------------------
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		for (auto & mesh : a_model->meshes)
		{
			//mesh.ensureVAO();

			unsigned int VAO = mesh.VAO;

			glBindVertexArray(VAO);
			//glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6); 
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}

	preRender();

	PhongShaderInstanced::getDefaultInstance()->bind(this, false);

	for (const auto & mesh : a_model->meshes)
    {
		glBindVertexArray(mesh.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);

		PhongShaderInstanced::getDefaultInstance()->setMaterial(mesh.material);

        glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);

        glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return m_frameBuffer->getTextureID();
}
