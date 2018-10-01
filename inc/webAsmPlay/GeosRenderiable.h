#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif // __EMSCRIPTEN__

#include <vector>
#include <glm/glm.hpp>
#include <tceGeom/vec3.h>

namespace geos
{
    namespace geom
    {
        class Geometry;
        class Polygon;
        class LineString;
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
                    const char    geomType);

    GeosRenderiable(const GLuint  vao,
                    const GLuint  vbo,
                    const GLuint  numVerts,
                    const char    geomType);

    static GeosRenderiable * prepairPolygon(const geos::geom::Polygon * poly);

    static GeosRenderiable * prepairLineString(const geos::geom::LineString * lineString);

    inline void renderPolygon(const glm::mat4 & MVP) const;
    inline void renderLineString(const glm::mat4 & MVP) const;

    const GLuint  vao;
    const GLuint  ebo;
    const GLuint  vbo;
    const int     numTriangles;
    const GLuint  numVerts;

    glm::vec4 fillColor;
    glm::vec4 outlineColor;

    const char geomType;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__