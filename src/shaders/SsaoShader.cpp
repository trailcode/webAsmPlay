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

#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/SsaoShader.h>

//REGISTER_SHADER(SsaoShader)

using namespace glm;

namespace
{
	ShaderProgram * shaderProgram   = NULL;
	SsaoShader	  * defaultInstance = NULL;

	GLuint points_buffer;

	struct SAMPLE_POINTS
	{
		vec4 point[256];
		vec4 random_vectors[256];
	};

	// Random number generator
	static unsigned int seed = 0x13371337;

	static inline float random_float()
	{
		float res;
		unsigned int tmp;

		seed *= 16807;

		tmp = seed ^ (seed >> 4) ^ (seed << 15);

		*((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

		return (res - 1.0f);
	}
}

SsaoShader* SsaoShader::getDefaultInstance() { return defaultInstance ;}

void SsaoShader::ensureShader()
{
	if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({	{GL_VERTEX_SHADER,		"SsaoShader.vs.glsl"	},
													{GL_FRAGMENT_SHADER,	"SsaoShader.fs.glsl"	}}),
											Variables({}),
											Variables({}));

	defaultInstance = new SsaoShader();
}

SsaoShader::SsaoShader() : Shader("SsaoShader")
{
	SAMPLE_POINTS point_data;

	for (size_t i = 0; i < 256; i++)
	{
		do
		{
			point_data.point[i][0] = random_float() * 2.0f - 1.0f;
			point_data.point[i][1] = random_float() * 2.0f - 1.0f;
			point_data.point[i][2] = random_float(); //  * 2.0f - 1.0f;
			point_data.point[i][3] = 0.0f;

		} while (length(point_data.point[i]) > 1.0f);

		normalize(point_data.point[i]);
	}

	for (size_t i = 0; i < 256; i++)
	{
		point_data.random_vectors[i][0] = random_float();
		point_data.random_vectors[i][1] = random_float();
		point_data.random_vectors[i][2] = random_float();
		point_data.random_vectors[i][3] = random_float();
	}

	glGenBuffers(1, &points_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);
}

SsaoShader::~SsaoShader()
{

}

GLuint SsaoShader::setColorTextureID		(const GLuint textureID) { return m_colorTextureID			= textureID ;}
GLuint SsaoShader::setNormalDepthTextureID	(const GLuint textureID) { return m_normalDepthTextureID	= textureID ;}

void SsaoShader::bind(	Canvas		* canvas,
						const bool    isOutline,
						const size_t  renderingStage)
{
	shaderProgram->bind();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorTextureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalDepthTextureID);
}