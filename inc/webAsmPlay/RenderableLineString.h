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

#ifndef __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__

#ifdef __EMSCRIPTEN__

    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <webAsmPlay/Renderable.h>

namespace geos
{
    namespace geom
    {
        class LineString;
    }
}

class RenderableLineString : public Renderable
{
public:

    ~RenderableLineString();

    static Renderable * create( const geos::geom::LineString    * lineString,
                                const glm::mat4                 & trans         = glm::mat4(1.0),
                                const glm::vec4                 & fillColor     = getDefaultFillColor(),
                                const glm::vec4                 & outlineColor  = getDefaultOutlineColor(),
                                const bool                        renderOutline = getDefaultRenderOutline(),
                                const bool                        renderFill    = getDefaultRenderFill());

    static Renderable * create( const ConstGeosGeomVec & lineStrings,
                                const glm::mat4        & trans         = glm::mat4(1.0),
                                const glm::vec4        & fillColor     = getDefaultFillColor(),
                                const glm::vec4        & outlineColor  = getDefaultOutlineColor(),
                                const bool               renderOutline = getDefaultRenderOutline(),
                                const bool               renderFill    = getDefaultRenderFill());

    void render(const glm::mat4 & MVP) const;

    static void ensureShaders();

private:

    RenderableLineString(   const GLuint      vao,
                            const GLuint      ebo,
                            const GLuint      vbo,
                            const GLuint      numElements,
                            const bool        isMulti,
                            const glm::vec4 & fillColor,
                            const glm::vec4 & outlineColor,
                            const bool        renderOutline,
                            const bool        renderFill);

    static Renderable * create( const FloatVec   & verts,
                                const Uint32Vec  & indices,
                                const glm::vec4  & fillColor,
                                const glm::vec4  & outlineColor,
                                const bool         renderOutline,
                                const bool         renderFill,
                                const bool         isMulti);

    const GLuint vao;
    const GLuint ebo;
    const GLuint vbo;
    const GLuint numElements;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__