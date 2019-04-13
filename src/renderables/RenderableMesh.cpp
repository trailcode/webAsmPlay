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
#include <webAsmPlay/renderables/RenderableMesh.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos;
using namespace geos::geom;

Renderable * RenderableMesh::create(	const ColoredExtrudedGeometryVec & polygons,
										const dmat4                      & trans,
										const bool                         showProgress)
{
	return create(getTesselations(	polygons,
									trans,
									showProgress));
}

Tessellations RenderableMesh::getTesselations(	const ColoredExtrudedGeometryVec & polygons,
												const dmat4                      & trans,
												const bool                         showProgress)
{
	Tessellations tessellations;

	getTesselations(tessellations,
					polygons,
					trans,
					showProgress);

	return tessellations;
}

void RenderableMesh::getTesselations(	Tessellations					 & tessellations,
										const ColoredExtrudedGeometryVec & polygons,
										const dmat4                      & trans,
										const bool                         showProgress)
{
	time_point<system_clock> startTime;

	if(showProgress) { startTime = system_clock::now() ;}

    for(size_t i = 0; i < polygons.size(); ++i)
    {
        if(showProgress) { doProgress("(6/6) Creating geometry:", i, polygons.size(), startTime) ;}

        const Geometry  * geom        = get<0>(polygons[i]);
        const GLuint      symbologyID = get<1>(polygons[i]);
        const double      height      = get<2>(polygons[i]);
        const double      minHeight   = get<3>(polygons[i]);
        
        const geom::Polygon  * poly;
        const MultiPolygon   * multiPoly;

        if((poly = dynamic_cast<const geom::Polygon *>(geom)))
        {
            tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID, height, minHeight));
            
            if((*tessellations.rbegin())->isEmpty())
            {
                dmess("Warning tessellation failed!");

                tessellations.pop_back();
            }
        }
        else if((multiPoly = dynamic_cast<const MultiPolygon *>(geom)))
        {
            dmess("Have a multiPoly!");

            //tesselateMultiPolygon(multiPoly, trans, tessellations);
            abort();
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
            abort();
        }
    }

    if(showProgress) { GUI::progress("", 1.0) ;}
}

Renderable * RenderableMesh::create(const Tessellations & tessellations)
{
    VertexArrayObject * vao = VertexArrayObject::create(tessellations);

    if(!vao)
    {
        dmess("Error! Could not create VertexArrayObject!");

        return NULL;
    };

    return new RenderableMesh(vao);
}

RenderableMesh::~RenderableMesh()
{
    delete m_vertexArrayObject;
}

RenderableMesh::RenderableMesh(VertexArrayObject * vertexArrayObject) : Renderable( true,
                                                                                    GUI::renderSettingsFillPolygons,
                                                                                    GUI::renderSettingsRenderPolygonOutlines),
                                                                        m_vertexArrayObject(vertexArrayObject)
{

}

void RenderableMesh::render(Canvas * canvas, const size_t renderStage)
{
    m_vertexArrayObject->bind(m_shader);
    m_vertexArrayObject->bindTriangles();

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if(getRenderFill() && m_shader->shouldRender(false, renderStage))
    {
        m_shader->bind(canvas, false, renderStage);

        m_vertexArrayObject->drawTriangles();
    }

    if(getRenderOutline() && m_shader->shouldRender(true, renderStage))
    {
        m_shader->bind(canvas, true, renderStage);

        m_vertexArrayObject->bindLines();
        
        m_vertexArrayObject->drawLines();
    }

	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void RenderableMesh::ensureVAO()
{
	m_vertexArrayObject->ensureVAO();
}