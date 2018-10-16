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

    static Renderiable * create(const geos::geom::Polygon   * poly,
                                const glm::mat4             & trans             = glm::mat4(1.0),
                                const glm::vec4             & fillColor         = glm::vec4(0.5,0.3,0,0.5),
                                const glm::vec4             & outlineColor      = glm::vec4(1,0,0,1),
                                const bool                    renderOutline     = true,
                                const bool                    renderFill        = true);

    static Renderiable * create(const geos::geom::MultiPolygon  * multyPoly,
                                const glm::mat4                 & trans         = glm::mat4(1.0),
                                const glm::vec4                 & fillColor     = glm::vec4(0.5,0.3,0,0.5),
                                const glm::vec4                 & outlineColor  = glm::vec4(1,0,0,1),
                                const bool                        renderOutline = true,
                                const bool                        renderFill    = true);

    static Renderiable * create(const std::vector<const geos::geom::Geometry *> & polygons,
                                const glm::mat4                                 & trans         = glm::mat4(1.0),
                                const glm::vec4                                 & fillColor     = glm::vec4(0.5,0.3,0,0.5),
                                const glm::vec4                                 & outlineColor  = glm::vec4(1,0,0,1),
                                const bool                                        renderOutline = true,
                                const bool                                        renderFill    = true);

    void render(const glm::mat4 & MVP) const;

    static void ensureShaders();

private:

    struct TesselationResult
    {
        double  * vertsOut          = NULL;
        int     * triangleIndices   = NULL;
        int       numVerts          = 0;
        int       numTriangles      = 0;

        std::vector<GLuint> counterVertIndices;
        std::vector<GLuint> counterVertIndices2;
    };

    RenderiablePolygon2D(   const GLuint                vao,
                            const GLuint                ebo,
                            const GLuint                ebo2,
                            const GLuint                vbo,
                            const int                   numTriangles,
                            const std::vector<GLuint> & counterVertIndices,
                            const size_t                numContourLines,
                            const bool                  isMulti,
                            const glm::vec4            & fillColor,
                            const glm::vec4            & outlineColor,
                            const bool                   renderOutline,
                            const bool                   renderFill);

    static Renderiable * createFromTesselations(const std::vector<const TesselationResult>  & tesselations,
                                                const glm::vec4                             & fillColor,
                                                const glm::vec4                             & outlineColor,
                                                const bool                                    renderOutline,
                                                const bool                                    renderFill);

    static TesselationResult tessellatePolygon(const geos::geom::Polygon  * poly, const glm::mat4 & trans);

    static void tesselateMultiPolygon(  const geos::geom::MultiPolygon       * multiPoly,
                                        const glm::mat4                      & trans,
                                        std::vector<const TesselationResult> & tesselationResults);

    static void ensureOutlineShader();

    const GLuint                vao;
    const GLuint                ebo;
    const GLuint                ebo2;
    const GLuint                vbo;
    const int                   numTriangles;
    const std::vector<GLuint>   counterVertIndices;
    const size_t                numContourLines;
}; 

#endif // __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
