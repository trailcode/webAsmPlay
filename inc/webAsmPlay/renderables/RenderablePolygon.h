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
#pragma once

#include <boost/geometry.hpp>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/geom/Tessellation.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/renderables/Renderable.h>

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

    static Renderable * create( const geos::geom::Polygon			* poly,
                                const glm::dmat4					& trans         = glm::dmat4(1.0),
                                const size_t						  symbologyID   = 0,
                                const AABB2D						& boxUV         = AABB2D());

    static Renderable * create( const geos::geom::MultiPolygon		* multyPoly,
                                const glm::dmat4					& trans         = glm::dmat4(1.0),
                                const size_t						  symbologyID   = 0,
                                const AABB2D						& boxUV         = AABB2D());

    static Renderable * create( const ColoredGeometryVec			& polygons,
                                const glm::dmat4					& trans         = glm::mat4(1.0),
                                const bool							  showProgress  = false);

	static Renderable * create( const boostGeom::Polygon			& polygon,
								const glm::dmat4					& trans         = glm::dmat4(1.0),
                                const size_t						  symbologyID   = 0,
                                const AABB2D						& boxUV         = AABB2D());

	static Renderable * create( const boostGeom::MultiPolygon		& multiPoly,
								const glm::dmat4					& trans         = glm::dmat4(1.0),
                                const size_t						  symbologyID   = 0,
                                const AABB2D						& boxUV         = AABB2D());

    void render(Canvas * canvas, const size_t renderStage = 0) override;

	void ensureVAO() override;

private:

    RenderablePolygon(VertexArrayObject * vertexArrayObject);
}; 

