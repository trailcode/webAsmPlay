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
#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;

// TODO Create better variable names. Try to simplify this class.

RenderablePolygon::RenderablePolygon(VertexArrayObject * vertexArrayObject) :   Renderable       (vertexArrayObject->isMulti()),
                                                                                vertexArrayObject(vertexArrayObject)
{
}

RenderablePolygon::~RenderablePolygon()
{
    delete vertexArrayObject;
}

Renderable * RenderablePolygon::create( const Polygon * poly,
                                        const dmat4   & trans,
                                        const size_t    symbologyID)
{
    Tessellations tesselations;

    tesselations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));

    if(!(*tesselations.begin())->vertsOut) { return NULL ;}

    return new RenderablePolygon(VertexArrayObject::create(tesselations));
}

Renderable * RenderablePolygon::create( const MultiPolygon  * multiPoly,
                                        const dmat4         & trans,
                                        const size_t          symbologyID)
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

        const Geometry  * geom        = get<0>(polygons[i]);
        const GLuint      symbologyID = get<1>(polygons[i]);
        
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if((poly = dynamic_cast<const Polygon *>(geom)))
        {
            tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));
            
            if(!(*tessellations.rbegin())->vertsOut)
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

    Renderable * ret = new RenderablePolygon(VertexArrayObject::create(tessellations));

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

void RenderablePolygon::render(const mat4 & MVP, const mat4 & MV) const
{
    vertexArrayObject->bind();

    vertexArrayObject->bindTriangles();

    GL_CHECK(glEnable(GL_BLEND));

    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glDisable(GL_DEPTH_TEST));

    if(getRenderFill())
    {
        shader->bind(MVP, MV, false);

        shader->enableVertexAttribArray(3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0); // TODO shader->bind should be doing this

        shader->enableColorAttribArray(1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        vertexArrayObject->drawTriangles();
    }

    if(getRenderOutline())
    {
        shader->bind(MVP, MV, true);

        // TODO this is here as a hack
        shader->enableVertexAttribArray(3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0); // TODO shader->bind should be doing this

        shader->enableColorAttribArray(1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        vertexArrayObject->bindLines();
        
        vertexArrayObject->drawLines();
    }

    glDisable(GL_BLEND); // TODO Remove!

    //glUseProgram(0);
}
