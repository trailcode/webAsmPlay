#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace geos
{
    namespace geom
    {
        class Geometry;
        class Polygon;
    }
}

class GeosRenderiable
{
public:

    virtual ~GeosRenderiable();

    void render(const glm::mat4 & MVP) const;

    static GeosRenderiable * create(const geos::geom::Geometry * geom);

private:

    GeosRenderiable(const GLuint  vao,
                    const GLuint  ebo,
                    const GLuint  vbo,
                    const int     numTriangles);

    static GeosRenderiable * prepairPolygon(const geos::geom::Polygon * poly);

    const GLuint  vao;
    const GLuint  ebo;
    const GLuint  vbo;
    const int     numTriangles;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__