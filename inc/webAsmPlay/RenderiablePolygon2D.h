#ifndef __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__

#include <vector>
#include <webAsmPlay/Renderiable.h>

namespace geos
{
    namespace geom
    {
        class Polygon;
        class MultiPolygon;
    }
}

class RenderiablePolygon2D : public Renderiable
{
public:

    ~RenderiablePolygon2D();

    static Renderiable * create(const geos::geom::Polygon * poly, const glm::mat4 & trans = glm::mat4(1.0));

    static Renderiable * create(const geos::geom::MultiPolygon * multyPoly, const glm::mat4 & trans = glm::mat4(1.0));

    static Renderiable * create(const std::vector<const geos::geom::Geometry *> & polygons, const glm::mat4 & trans = glm::mat4(1.0));

    void render(const glm::mat4 & MVP) const;

private:

    struct TesselationResult
    {
        double  * vertsOut          = NULL;
        int     * triangleIndices   = NULL;
        int       numVerts          = 0;
        int       numTriangles      = 0;

        std::vector<GLuint> counterVertIndices;
    };

    RenderiablePolygon2D(   const GLuint                vao,
                            const GLuint                ebo,
                            const GLuint                vbo,
                            const int                   numTriangles,
                            const std::vector<GLuint> & counterVertIndices);

    static Renderiable * createFromTesselations(const std::vector<const TesselationResult> & tesselations);

    static TesselationResult tessellatePolygon(const geos::geom::Polygon  * poly, const glm::mat4 & trans);

    static void tesselateMultiPolygon(  const geos::geom::MultiPolygon       * multiPoly,
                                        const glm::mat4                      & trans,
                                        std::vector<const TesselationResult> & tesselationResults);

    const GLuint                vao;
    const GLuint                ebo;
    const GLuint                vbo;
    const int                   numTriangles;
    const std::vector<GLuint>   counterVertIndices;
}; 

#endif // __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
