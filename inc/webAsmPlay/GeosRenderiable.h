#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace geos
{
    namespace geom
    {
        class Polygon;
    }
}

class GeosRenderiable
{
public:

    GeosRenderiable(geos::geom::Polygon * poly);

    virtual ~GeosRenderiable();

    void render(const glm::mat4 & MVP) const;

private:

    GLuint  vao;
    GLuint  ebo;
    GLuint  vbo;
    int numTriangles;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__