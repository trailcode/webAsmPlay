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

#ifndef __WEB_ASM_PLAY__DEFERRED_RENDERABLE_H__
#define __WEB_ASM_PLAY__DEFERRED_RENDERABLE_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
#endif

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <webAsmPlay/Renderable.h>

class DeferredRenderable : public Renderable
{
public:

    virtual ~DeferredRenderable();

    static DeferredRenderable * createFromQueued(); 

    static void addQuadrangle(  const glm::vec3 & A,
                                const glm::vec3 & B,
                                const glm::vec3 & C,
                                const glm::vec3 & D,
                                const glm::vec4 & color);

    static void addTriangle(const glm::vec3 & A,
                            const glm::vec3 & B,
                            const glm::vec3 & C,
                            const glm::vec4 & color);

    static void addLine(const glm::vec3 & A,
                        const glm::vec3 & B,
                        const glm::vec4 & color);

    void render(const glm::mat4 & MVP, const glm::mat4 & MV) const;

private:

    DeferredRenderable( const GLuint & vao,
                        const GLuint & ebo,
                        const GLuint & ebo2,
                        const GLuint & vbo,
                        const GLuint & numTriIndices,
                        const GLuint & numLineIndices);

    const GLuint vao;
    const GLuint ebo;
    const GLuint ebo2;
    const GLuint vbo;
    const GLuint numTriIndices;
    const GLuint numLineIndices;
};

#endif // __WEB_ASM_PLAY__DEFERRED_RENDERABLE_H__