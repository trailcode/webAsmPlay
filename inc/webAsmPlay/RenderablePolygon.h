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

#ifndef __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
#define __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__

#ifdef __EMSCRIPTEN__

    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <vector>
#include <webAsmPlay/Renderable.h>

namespace geos
{
    namespace geom
    {
        class Polygon;
        class MultiPolygon;
    }
}

class RenderablePolygon : public Renderable
{
public:

    ~RenderablePolygon();

    static Renderable * create( const geos::geom::Polygon * poly,
                                const glm::dmat4          & trans       = glm::dmat4(1.0),
                                const size_t                symbologyID = 0);

    static Renderable * create( const geos::geom::MultiPolygon  * multyPoly,
                                const glm::dmat4                & trans         = glm::dmat4(1.0),
                                const size_t                      symbologyID   = 0);

    static Renderable * create( const ColoredGemetryVec & polygons,
                                const glm::dmat4        & trans        = glm::mat4(1.0),
                                const bool                showProgress = false);

    void render(const glm::mat4 & MVP, const glm::mat4 & MV) const;

private:

    struct TessellationResult
    {
        double  * vertsOut          = NULL;
        int     * triangleIndices   = NULL;
        int       numVerts          = 0;
        int       numTriangles      = 0;

        Uint32Vec counterVertIndices;
        Uint32Vec counterVertIndices2;

        GLuint symbologyID;
    };

    RenderablePolygon(  const GLuint      vao,
                        const GLuint      ebo,
                        const GLuint      ebo2,
                        const GLuint      vbo,
                        const int         numTriangles,
                        const Uint32Vec & counterVertIndices,
                        const size_t      numContourLines,
                        const bool        isMulti);

    typedef std::vector<const TessellationResult> Tessellations;

    static Renderable * createFromTessellations(const Tessellations & tessellations);

    static TessellationResult tessellatePolygon(const geos::geom::Polygon * poly,
                                                const glm::dmat4          & trans,
                                                const size_t                symbologyID);

    static void tessellateMultiPolygon( const geos::geom::MultiPolygon  * multiPoly,
                                        const glm::dmat4                & trans,
                                        Tessellations                   & tessellationResults,
                                        const size_t                      symbologyID);

    const GLuint    vao;
    const GLuint    ebo;
    const GLuint    ebo2;
    const GLuint    vbo;
    const int       numTriangles;
    const Uint32Vec counterVertIndices;
    const size_t    numContourLines;
}; 

#endif // __WEB_ASM_PLAY_RENDERIABLE_POLYGON2D_H__
