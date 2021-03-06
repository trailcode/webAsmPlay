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

#ifdef WIN32
#define Polygon Polygon_Custom // Prevent Polygon ambiguity
#include <windows.h>
#undef Polygon
#endif

#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/renderables/Renderable.h>

using namespace std;
using namespace glm;
using namespace geos;
using namespace geos::geom;

Renderable * Renderable::create(const Geometry::Ptr & geom,
                                const dmat4         & trans,
                                const AABB2D        & boxUV,
								const bool			  swapUV_Axis)
{
    return create(geom.get(), trans, boxUV, swapUV_Axis);
}

Renderable * Renderable::create(const Geometry * geom,
                                const dmat4    & trans,
                                const AABB2D   & boxUV,
								const bool		 swapUV_Axis)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:				return RenderablePoint::create(dynamic_cast<const Point *>(geom), trans);
        case GEOS_LINESTRING:           
        case GEOS_LINEARRING:           return RenderableLineString::create(dynamic_cast<const LineString *>(geom), trans);
		case GEOS_POLYGON:              return RenderablePolygon   ::create(dynamic_cast<const Polygon    *>(geom), trans, 0, boxUV, swapUV_Axis);
        case GEOS_MULTIPOINT:           dmess("Implement me!"); return nullptr;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return nullptr;
        case GEOS_MULTIPOLYGON:         return RenderablePolygon::create(   dynamic_cast<const MultiPolygon *>(geom), trans, 0, boxUV, swapUV_Axis);
        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return nullptr;
        default: dmessError("Error!");
    }

    return nullptr;
}

Renderable * Renderable::create(const boostGeom::Polygon		& polygon,
								const dmat4						& trans,
                                const size_t					  symbologyID,
                                const AABB2D					& boxUV,
								const bool						  swapUV_Axis)
{
	return RenderablePolygon::create(polygon, trans, symbologyID, boxUV, swapUV_Axis);
}

Renderable * Renderable::create(const boostGeom::MultiPolygon	& multiPoly,
								const dmat4						& trans,
								const size_t					  symbologyID,
								const AABB2D					& boxUV,
								const bool						  swapUV_Axis)
{
	return RenderablePolygon::create(multiPoly, trans, symbologyID, boxUV, swapUV_Axis);
}

Renderable::Renderable( const bool isMulti,
                        const bool renderFill,
                        const bool renderOutline) : m_isMulti       (isMulti),
                                                    m_renderFill    (renderFill),
                                                    m_renderOutline (renderOutline),
                                                    m_shader        (ColorShader::getDefaultInstance())
{
}

Renderable::Renderable( VertexArrayObject	* vertexArrayObject,
						const bool			  renderFill,
						const bool			  renderOutline) :	m_vertexArrayObject	(vertexArrayObject),
																m_isMulti			(vertexArrayObject->isMulti()),
																m_renderFill		(renderFill),
																m_renderOutline		(renderOutline),
																m_shader			(ColorShader::getDefaultInstance())
{
}

Renderable::~Renderable()
{
    for(OnDelete & callback : m_DeleteCallbacks) { callback(this) ;}

	delete m_vertexArrayObject;
}

void Renderable::addOnDeleteCallback(const OnDelete & callback) { m_DeleteCallbacks.push_back(callback) ;}

Shader * Renderable::getShader() const { return m_shader ;}

Renderable * Renderable::setShader(Shader * shader)
{
	m_shader = shader;

	return this;
}

bool Renderable::getRenderFill()    const { return m_renderFill ;}
bool Renderable::getRenderOutline() const { return m_renderOutline ;}

Renderable * Renderable::setRenderFill   (const bool render)
{
	m_renderFill = render;

	return this;
}

Renderable * Renderable::setRenderOutline(const bool render)
{
	m_renderOutline = render;

	return this;
}

size_t Renderable::getNumTriangles() const
{
	if(!m_vertexArrayObject) { return 0 ;}

	return m_vertexArrayObject->getNumTriangles();
}

void Renderable::render(const mat4		& model,
						const mat4		& view,
						const mat4		& projection,
						const size_t	  renderStage)
{
	dmessError("Implement me!");
}