#ifndef __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__

#include <webAsmPlay/Renderiable.h>

class RenderiablePolygon2D : public Renderiable
{
public:

    ~RenderiablePolygon2D();

    static Renderiable * create(const geos::geom::Polygon * poly);

    void render(const glm::mat4 & MVP) const;

private:

    RenderiablePolygon2D(   const GLuint                vao,
                            const GLuint                ebo,
                            const GLuint                vbo,
                            const int                   numTriangles,
                            const std::vector<size_t> & counterVertIndices);

    const GLuint                vao;
    const GLuint                ebo;
    const GLuint                vbo;
    const int                   numTriangles;
    const std::vector<size_t>   counterVertIndices;
}; 

#endif // __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
