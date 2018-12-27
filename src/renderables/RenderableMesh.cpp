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

Renderable * RenderableMesh::create( const ColoredExtrudedGeometryVec & polygons,
                                     const dmat4                      & trans,
                                     const bool                         showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    Tessellations tessellations;

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

    Renderable * ret = createFromTessellations(tessellations);

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

Renderable * RenderableMesh::createFromTessellations(const Tessellations & tessellations)
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
    delete vertexArrayObject;
}

RenderableMesh::RenderableMesh(VertexArrayObject * vertexArrayObject) : Renderable( true,
                                                                                    GUI::renderSettingsFillPolygons,
                                                                                    GUI::renderSettingsRenderPolygonOutlines),
                                                                        vertexArrayObject(vertexArrayObject)
{

}

void RenderableMesh::render(Canvas * canvas, const size_t renderStage) const
{
    vertexArrayObject->bind(shader);
    vertexArrayObject->bindTriangles();

    shader->setVertexArrayFormat(ArrayFormat(3, 7 * sizeof(GLfloat), 0));
    shader->setColorArrayFormat (ArrayFormat(1, 7 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))));
    shader->setNormalArrayFormat(ArrayFormat(3, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));

    if(getRenderFill() && shader->shouldRender(false, renderStage))
    {
        shader->bind(canvas, false, renderStage);

        vertexArrayObject->drawTriangles();
    }

    if(getRenderOutline() && shader->shouldRender(true, renderStage))
    {
        shader->bind(canvas, true, renderStage);

        vertexArrayObject->bindLines();
        
        vertexArrayObject->drawLines();
    }
}
