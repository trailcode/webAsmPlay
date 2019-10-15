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
#include <webAsmPlay/renderables/Model.h>
#include <webAsmPlay/renderables/RenderableModelInstanced.h>

using namespace std;
using namespace glm;

Renderable * RenderableModelInstanced::create(const string & modelPath, const vector<vec2> & modelPositions)
{
	return new RenderableModelInstanced(modelPath, modelPositions);
}

RenderableModelInstanced::RenderableModelInstanced(const string & modelPath, const vector<vec2> & modelPositions)
{
	m_model = new Model(modelPath);

	vector<mat4> modelMatrices(modelPositions.size());

	for(size_t i = 0; i < modelPositions.size(); ++i)
	{
		modelMatrices[i] = translate(modelMatrices[i], glm::vec3(modelPositions[i], 0));

		modelMatrices[i] = scale(modelMatrices[i], vec3(0.001, 0.001, 0.001));

		modelMatrices[i] = rotate(modelMatrices[i], radians(90.0f), vec3(1,0,0));
	}

	glGenBuffers(1, &m_modelInstancedID);

    glBindBuffer(GL_ARRAY_BUFFER, m_modelInstancedID);

    glBufferData(GL_ARRAY_BUFFER, modelPositions.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
}

RenderableModelInstanced::~RenderableModelInstanced()
{
	delete m_model;

	glDeleteBuffers(1, &m_modelInstancedID);
}

void RenderableModelInstanced::render(Canvas * canvas, const size_t renderStage)
{
	
}

void RenderableModelInstanced::ensureVAO()
{
	if(m_didVAO) { return ;}

	m_didVAO = true;

	for (auto & mesh : m_model->meshes)
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
