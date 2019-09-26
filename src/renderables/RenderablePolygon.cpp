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

#include <chrono>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos;
using namespace geos::geom;

RenderablePolygon::RenderablePolygon(VertexArrayObject * vertexArrayObject) :   Renderable(			vertexArrayObject,
																									GUI::s_renderSettingsFillPolygons,
																									GUI::s_renderSettingsRenderPolygonOutlines)
{
}

RenderablePolygon::~RenderablePolygon()
{
}

Renderable * RenderablePolygon::create( const Polygon * poly,
                                        const dmat4   & trans,
                                        const size_t    symbologyID,
                                        const AABB2D  & boxUV)
{
    Tessellations tesselations;

    tesselations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));

    if((*tesselations.begin())->isEmpty()) { return nullptr ;}

    if(get<0>(boxUV) != get<2>(boxUV))
    {
		const dvec2 min = trans * dvec4(get<0>(boxUV), get<1>(boxUV), 0, 1);
		const dvec2 max = trans * dvec4(get<2>(boxUV), get<3>(boxUV), 0, 1);

		if (auto vao = VertexArrayObject::create(tesselations, AABB2D(min.x, min.y, max.x, max.y))) { return new RenderablePolygon(vao) ;}

        return nullptr;
    }

	if(auto vao = VertexArrayObject::create(tesselations)) { new RenderablePolygon(vao) ;}

    return nullptr;
}

Renderable * RenderablePolygon::create( const MultiPolygon  * multiPoly,
                                        const dmat4         & trans,
                                        const size_t          symbologyID,
                                        const AABB2D        & boxUV // TODO Implement!
                                        )
{
    Tessellations tessellations;

    Tessellation::tessellateMultiPolygon(multiPoly, trans, tessellations, symbologyID);

    return new RenderablePolygon(VertexArrayObject::create(tessellations));
}

Renderable * RenderablePolygon::create( const ColoredGeometryVec & polygons,
                                        const dmat4              & trans,
                                        const bool                 showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    Tessellations tessellations;

    for(size_t i = 0; i < polygons.size(); ++i)
    {
        if(showProgress) { doProgress("(6/6) Creating geometry:", i, polygons.size(), startTime) ;}

        const Geometry  * geom        =			get<0>(polygons[i]);
        const GLuint      symbologyID = (GLuint)get<1>(polygons[i]);
        
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if((poly = dynamic_cast<const Polygon *>(geom)))
        {
            tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));
            
            if((*tessellations.rbegin())->isEmpty())
            {
                dmess("Warning tessellation failed!");

                tessellations.pop_back();
            }
        }
        else if((multiPoly = dynamic_cast<const MultiPolygon *>(geom))) { dmessError("Have a multiPoly!")						;}
        else															{ dmessError("Warning not a polygon or multi-polygon.") ;}
    }

    Renderable * ret = new RenderablePolygon(VertexArrayObject::create(tessellations));

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

Renderable * RenderablePolygon::create( const boostGeomUtil::MultiPolygon	& multiPoly,
										const dmat4							& trans,
										const size_t						  symbologyID,
										const AABB2D						& boxUV)
{
	return nullptr;
}

void RenderablePolygon::render(Canvas * canvas, const size_t renderStage)
{
	const bool renderFill		= getRenderFill()		&& m_shader->m_shouldRender(false,	renderStage);
	const bool renderOutline	= getRenderOutline()	&& m_shader->m_shouldRender(true,	renderStage);

	if(!renderFill && !renderOutline) { return ;}

    m_vertexArrayObject->bind(m_shader);

    if(renderFill)
    {
        m_shader->bind(canvas, false, renderStage);
		
		m_vertexArrayObject->bindTriangles();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        m_vertexArrayObject->drawTriangles();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if(renderOutline)
    {
        m_shader->bind(canvas, true, renderStage);

        m_vertexArrayObject->bindLines();
        
        m_vertexArrayObject->drawLines();
    }
}

void RenderablePolygon::ensureVAO() { m_vertexArrayObject->ensureVAO() ;}