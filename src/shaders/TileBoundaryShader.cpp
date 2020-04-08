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
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/TileBoundaryShader.h>

//REGISTER_SHADER(TileBoundaryShader)

namespace
{
    ShaderProgram		* a_shaderProgram   = nullptr;
    TileBoundaryShader	* a_defaultInstance = nullptr;

    GLint a_vertInAttr;
    
    GLint a_MVP;
}

TileBoundaryShader * TileBoundaryShader::getDefaultInstance() { return a_defaultInstance ;}

void TileBoundaryShader::ensureShader()
{
	//return; // This one is not compiling!
#ifndef __EMSCRIPTEN__

    if(a_shaderProgram) { return ;}

	a_shaderProgram = ShaderProgram::create(GLSL({		{GL_VERTEX_SHADER,		"TileBoundaryShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"TileBoundaryShader.fs.glsl"	},
														{GL_GEOMETRY_SHADER,	"TileBoundaryShader.gs.glsl"	}}),
                                            Variables({	{"vertIn",				a_vertInAttr					}}),
                                            Variables({	{"MVP",					a_MVP							}}));

    a_defaultInstance = new TileBoundaryShader();

	#else
	dmess("Fix!");
	#endif
}

TileBoundaryShader::TileBoundaryShader() : Shader(	"TileBoundaryShader",
													nullptr,
													Shader::s_defaultShouldRender)
{

}

TileBoundaryShader::~TileBoundaryShader()
{

}

void TileBoundaryShader::bind(  Canvas     * canvas,
                                const bool   isOutline,
                                const size_t renderingStage)
{
#ifndef __EMSCRIPTEN__
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	a_shaderProgram->bind();

	a_shaderProgram->setUniform(a_MVP, canvas->getMVP_Ref());
	#else
	dmess("Fix!");
	#endif
}