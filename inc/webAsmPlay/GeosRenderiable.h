#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <tceGeom/vec3.h>

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

    glm::vec4 setFillColor(const glm::vec4 & fillColor);
    glm::vec4 getFillColor() const;

    glm::vec4 setOutlineColor(const glm::vec4 & outlineColor);
    glm::vec4 getOutlineColor() const;

private:

    GeosRenderiable(const GLuint  vao,
                    const GLuint  ebo,
                    const GLuint  vbo,
                    const int     numTriangles,
                    const std::vector<size_t> & counterVertIndices);

    static GeosRenderiable * prepairPolygon(const geos::geom::Polygon * poly);

    const GLuint  vao;
    const GLuint  ebo;
    const GLuint  vbo;
    const int     numTriangles;

    const std::vector<size_t> counterVertIndices;

    glm::vec4 fillColor;
    glm::vec4 outlineColor;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__