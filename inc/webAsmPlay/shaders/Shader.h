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

#ifndef __WEB_ASM_PLAY_SHADER_H__
#define __WEB_ASM_PLAY_SHADER_H__

#include <glm/mat4x4.hpp>
#include <webAsmPlay/OpenGL_Util.h>

class Canvas;

class Shader
{
public:

    virtual void bind(Canvas * canvas, const bool isOutline, const size_t renderingStage) = 0;

    void setVertexArrayFormat(  const GLint     size    = 2,
                                const GLsizei   stride  = 0,
                                const GLvoid  * pointer = NULL);

    void setColorArrayFormat(   const GLint     size    = 2,
                                const GLsizei   stride  = 0,
                                const GLvoid  * pointer = NULL);

    void setNormalArrayFormat(  const GLint     size    = 2,
                                const GLsizei   stride  = 0,
                                const GLvoid  * pointer = NULL);

    std::string getName() const;

    virtual size_t getNumRenderingStages() const;

    virtual bool shouldRender(const bool isOutline, const size_t renderingStage) const;

protected:

    Shader( const std::string & shaderName);

    virtual ~Shader() {}

    const std::string shaderName;

    GLint            sizeVertex     = 2;
    GLsizei          strideVertex   = 0;
    const GLvoid   * pointerVertex  = NULL;

    GLint            sizeNormal     = 2;
    GLsizei          strideNormal   = 0;
    const GLvoid   * pointerNormal  = NULL;

    GLint            sizeColor      = 2;
    GLsizei          strideColor    = 0;
    const GLvoid   * pointerColor   = NULL;
};

#endif // __WEB_ASM_PLAY_SHADER_H__