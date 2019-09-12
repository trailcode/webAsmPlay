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
    ShaderProgram		* shaderProgram   = nullptr;
    TileBoundaryShader	* defaultInstance = nullptr;

    GLint vertInAttrLoc;
    
    GLint MVP_Loc;
}

TileBoundaryShader * TileBoundaryShader::getDefaultInstance()
{
    return defaultInstance;
}

void TileBoundaryShader::ensureShader()
{
	return; // This one is not compiling!

    if(shaderProgram) { return ;}

	shaderProgram = ShaderProgram::create(  GLSL({		{GL_VERTEX_SHADER,		"TileBoundaryShader.vs.glsl"	},
														{GL_FRAGMENT_SHADER,	"TileBoundaryShader.fs.glsl"	},
														{GL_GEOMETRY_SHADER,	"TileBoundaryShader.gs.glsl"	}}),
                                            Variables({	{"vertIn",				vertInAttrLoc					}}),
                                            Variables({	{"MVP",					MVP_Loc							}}));

    defaultInstance = new TileBoundaryShader();
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

}