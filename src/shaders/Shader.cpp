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

#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/Shader.h>

using namespace std;
using namespace glm;

Shader::Shader( const string  & shaderName,
                ShaderProgram * program,
                const GLint     vertexInLoc,
                const GLint     colorInLoc,
                const GLint     normalInLoc) :  shaderName  (shaderName),
                                                program     (program),
                                                vertexInLoc (vertexInLoc),
                                                colorInLoc  (colorInLoc),
                                                normalInLoc (normalInLoc){}

void Shader::enableVertexArray(const GLint       size,
                               const GLenum      type,
                               const GLboolean   normalized,
                               const GLsizei     stride,
                               const GLvoid    * pointer)
{
    ShaderProgram::enableVertexAttribArray(vertexInLoc,
                                           size,
                                           type,
                                           normalized,
                                           stride,
                                           pointer);
}

void Shader::enableColorArray( const GLint       size,
                               const GLenum      type,
                               const GLboolean   normalized,
                               const GLsizei     stride,
                               const GLvoid    * pointer)
{
    ShaderProgram::enableVertexAttribArray(colorInLoc,
                                           size,
                                           type,
                                           normalized,
                                           stride,
                                           pointer);
}

void Shader::enableNormalArray( const GLint       size,
                                const GLenum      type,
                                const GLboolean   normalized,
                                const GLsizei     stride,
                                const GLvoid    * pointer)
{
    ShaderProgram::enableVertexAttribArray(normalInLoc,
                                           size,
                                           type,
                                           normalized,
                                           stride,
                                           pointer);
}

string Shader::getName() const { return shaderName ;}