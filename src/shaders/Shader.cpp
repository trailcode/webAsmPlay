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

#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/Shader.h>

using namespace std;
using namespace glm;

Shader::Shader(const string  & shaderName) :    shaderName      (shaderName),
                                                colorSymbology  (ColorSymbology::getInstance("defaultPolygon")) {}

void Shader::setVertexArrayFormat(const ArrayFormat & vertexFormat) { this->vertexFormat = vertexFormat ;}
void Shader::setNormalArrayFormat(const ArrayFormat & normalFormat) { this->normalFormat = normalFormat ;}
void Shader::setColorArrayFormat (const ArrayFormat & colorFormat)  { this->colorFormat  = colorFormat  ;}
void Shader::setUV_ArrayFormat   (const ArrayFormat & uvFormat)     { this->uvFormat     = uvFormat     ;}

string Shader::getName() const { return shaderName ;}

size_t Shader::getNumRenderingStages() const { return 1 ;}

bool Shader::shouldRender(const bool isOutline, const size_t renderingStage) const { return renderingStage == 0 ;}

ColorSymbology * Shader::setColorSymbology(ColorSymbology * colorSymbology) { return this->colorSymbology = colorSymbology ;}
ColorSymbology * Shader::getColorSymbology() const { return colorSymbology ;}