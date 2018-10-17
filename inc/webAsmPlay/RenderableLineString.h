#ifndef __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__

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

    void render(const glm::mat4 & MVP) const;

private:

    RenderableLineString(   const GLuint      vao,
                            const GLuint      ebo,
                            const GLuint      vbo,
                            const GLuint      numVerts,
                            const bool        isMulti,
                            const glm::vec4 & fillColor,
                            const glm::vec4 & outlineColor,
                            const bool        renderOutline,
                            const bool        renderFill);

    const GLuint vao;
    const GLuint ebo;
    const GLuint vbo;
    const GLuint numVerts;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__