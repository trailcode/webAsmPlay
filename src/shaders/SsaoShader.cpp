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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/SsaoShader.h>

//REGISTER_SHADER(SsaoShader)

using namespace glm;

namespace
{
	ShaderProgram * a_shaderProgram   = nullptr;
	SsaoShader	  * a_defaultInstance = nullptr;

	GLuint a_points_buffer;

	GLint a_ssaoRadius;
	GLint a_pointCount;
	GLint a_minDepth;
	GLint a_mixPercent;

	struct SAMPLE_POINTS
	{
		vec4 m_point[256];
		vec4 m_random_vectors[256];
	};

	// Random number generator // TODO, use a lib function
	static inline float random_float()
	{
		float res;
		unsigned int tmp;

		static unsigned int seed = 0x13371337;

		seed *= 16807;

		tmp = seed ^ (seed >> 4) ^ (seed << 15);

		*((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

		return (res - 1.0f);
	}
}

SsaoShader* SsaoShader::getDefaultInstance() { return a_defaultInstance ;}

void SsaoShader::ensureShader()
{
	if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"SsaoShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"SsaoShader.fs.glsl"	}}),
											Variables({}),
											Variables({	{"ssaoRadius",			a_ssaoRadius			},
														{"pointCount",			a_pointCount			},
														{"minDepth",			a_minDepth				},
														{"mixPercent",			a_mixPercent			}}));

	a_defaultInstance = new SsaoShader();
}

SsaoShader::SsaoShader() : Shader(	"SsaoShader",
									nullptr,
									Shader::s_defaultShouldRender)
{
	SAMPLE_POINTS point_data;

	for (size_t i = 0; i < 256; i++)
	{
		do
		{
			point_data.m_point[i][0] = random_float() * 2.0f - 1.0f;
			point_data.m_point[i][1] = random_float() * 2.0f - 1.0f;
			point_data.m_point[i][2] = random_float(); //  * 2.0f - 1.0f;
			point_data.m_point[i][3] = 0.0f;

		} while (length(point_data.m_point[i]) > 1.0f);

		point_data.m_point[i] = normalize(point_data.m_point[i]);
	}

	for (size_t i = 0; i < 256; i++)
	{
		point_data.m_random_vectors[i][0] = random_float();
		point_data.m_random_vectors[i][1] = random_float();
		point_data.m_random_vectors[i][2] = random_float();
		point_data.m_random_vectors[i][3] = random_float();
	}

	glGenBuffers(1, &a_points_buffer);

	glBindBuffer(GL_UNIFORM_BUFFER, a_points_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);
}

SsaoShader::~SsaoShader()
{

}

GLuint SsaoShader::setColorTextureID		(const GLuint textureID) { return m_colorTextureID			= textureID ;}
GLuint SsaoShader::setNormalDepthTextureID	(const GLuint textureID) { return m_normalDepthTextureID	= textureID ;}

float SsaoShader::setSSAO_Radius(const float radius)
{
	//shaderProgram->setUniformf(ssaoRadius, radius);

	return m_SSAO_Radius = radius;
}

float SsaoShader::getSSAO_Radius() const { return m_SSAO_Radius; }

GLuint SsaoShader::setNumPoints(const GLuint num)		{ return m_numPoints = num; }
GLuint SsaoShader::getNumPoints() const					{ return m_numPoints; }

float SsaoShader::setMinDepth(const float minDepth)		{ return m_minDepth = minDepth; }
float SsaoShader::getMinDepth() const					{ return m_minDepth; }

float SsaoShader::setMixPercent(const float percent)	{ return m_mixPercent = percent; }
float SsaoShader::getMixPercent() const					{ return m_mixPercent; }

void SsaoShader::bind(	Canvas		* canvas,
						const bool    isOutline,
						const size_t  renderingStage)
{
	a_shaderProgram->bind();

	a_shaderProgram->setUniformf(a_ssaoRadius,	m_SSAO_Radius);
	a_shaderProgram->setUniformi(a_pointCount,	m_numPoints);
	a_shaderProgram->setUniformf(a_minDepth,	m_minDepth);
	a_shaderProgram->setUniformf(a_mixPercent,	m_mixPercent);

	glDisable(GL_DEPTH_TEST);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, a_points_buffer);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_colorTextureID);

	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_normalDepthTextureID);
}