#ifndef __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__

#include <webAsmPlay/Renderiable.h>

namespace geos
{
    namespace geom
    {
        class LineString;
    }
}

class RenderiableLineString2D : public Renderiable
{
public:

    ~RenderiableLineString2D();

    static Renderiable * create(const geos::geom::LineString * lineString, const glm::mat4 & trans = glm::mat4(1.0));

    void render(const glm::mat4 & MVP) const;

private:

    RenderiableLineString2D(const GLuint vao,
                            const GLuint vbo,
                            const GLuint numVerts);

    const GLuint vao;
    const GLuint vbo;
    const GLuint numVerts;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_LINE_STRING_2D_H__