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

#ifndef __WEB_ASM_PLAY_RENDERABLE_POINT_H__
#define __WEB_ASM_PLAY_RENDERABLE_POINT_H__

#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/renderables/Renderable.h>

class RenderablePoint : public Renderable
{
public:

    ~RenderablePoint();

    static Renderable * create( const glm::vec3 & pos,
                                const glm::mat4 & trans = glm::mat4(1.0));

    static Renderable * create( const ConstGeosGeomVec & points,
                                const glm::mat4        & trans        = glm::mat4(1.0),
                                const bool               showProgress = false);

    void render(Canvas * canvas) const;

private:

    RenderablePoint(const GLuint vao,
                    const GLuint ebo,
                    const GLuint vbo,
                    const bool   isMulti);

    const GLuint vao;
    const GLuint ebo;
    const GLuint vbo;
};

#endif // __WEB_ASM_PLAY_RENDERABLE_POINT_H__
