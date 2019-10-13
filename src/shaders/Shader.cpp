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
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/shaders/SkyBoxShader.h>
#include <webAsmPlay/shaders/SsaoShader.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/shaders/NormalToRGB_Shader.h>
#include <webAsmPlay/shaders/DepthToRGB_Shader.h>
#include <webAsmPlay/shaders/TileBoundaryShader.h>
#include <webAsmPlay/shaders/PhongShader.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/Shader.h>

using namespace std;
using namespace glm;

vector<function<void()>> Shader::s_shadersToRegister;

Shader::ShouldRenderFunctor Shader::s_defaultShouldRender = {[](const bool isOutline, const size_t renderingStage)
{
	return renderingStage == POST_G_BUFFER;
}};

Shader::Shader(	const string				& shaderName,
				ColorSymbology				* colorSymbology,
				const ShouldRenderFunctor	& shouldRenderFunctor) :    m_shaderName		(shaderName),
																		m_colorSymbology	(colorSymbology ? colorSymbology : ColorSymbology::getInstance("defaultPolygon")),
																		m_shouldRender		(shouldRenderFunctor) {}

string Shader::getName() const { return m_shaderName ;}

size_t Shader::getNumRenderingStages() const { return 1 ;}

ColorSymbology * Shader::setColorSymbology(ColorSymbology * colorSymbology) { return m_colorSymbology = colorSymbology	;}
ColorSymbology * Shader::getColorSymbology() const							{ return m_colorSymbology					;}

RegisterShader::RegisterShader(const function<void()> & registerFunction)
{
	dmess("RegisterShader");

	Shader::s_shadersToRegister.push_back(registerFunction);
}

void Shader::ensureShaders()
{
	BindlessTextureShader::ensureShader();
	ColorDistanceDepthShader3D::ensureShader();
	ColorDistanceShader::ensureShader();
	ColorShader::ensureShader();
	ColorVertexShader::ensureShader();
	SkyBoxShader::ensureShader();
	SsaoShader::ensureShader();
	TextureShader::ensureShader();
	NormalToRGB_Shader::ensureShader();
	DepthToRGB_Shader::ensureShader();
	TileBoundaryShader::ensureShader();
	PhongShader::ensureShader();

	for (const auto& i : s_shadersToRegister)
	{
		// i(); // This does not seem to work.
	}
}