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
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <webAsmPlay/OpenGL_Util.h>

class Canvas;
class ColorSymbology;

#define REGISTER_SHADER(a) namespace { static RegisterShader __([]() { ##a::ensureShader() ;}) ;}

class RegisterShader
{
public:

	RegisterShader(const std::function<void()>& registerFunction);

private:
};

class Shader
{
public:

    virtual void bind(Canvas * canvas, const bool isOutline, const size_t renderingStage) = 0;

    std::string getName() const;

    virtual size_t getNumRenderingStages() const;

    ColorSymbology * setColorSymbology(ColorSymbology * colorSymbology);
    ColorSymbology * getColorSymbology() const;

	static void ensureShaders();

	typedef std::function<bool(const bool isOutline, const size_t renderingStage)> ShouldRenderFunctor;

	const ShouldRenderFunctor m_shouldRender;
	
protected:

	static ShouldRenderFunctor s_defaultShouldRender;

    Shader(	const std::string			& shaderName,
			ColorSymbology				* colorSymbology = nullptr,
			const ShouldRenderFunctor	& shouldRenderFunctor = s_defaultShouldRender);

    virtual ~Shader() {}

    const std::string m_shaderName;

    ColorSymbology * m_colorSymbology = nullptr;

	friend class RegisterShader;

	static std::vector<std::function<void()>> s_shadersToRegister;
};



