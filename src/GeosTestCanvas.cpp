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

#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/RenderableLineString.h>
#include <webAsmPlay/GeosTestCanvas.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

GeosTestCanvas::GeosTestCanvas() :  buffer1(-1),
                                    buffer2(-1),
                                    buffer3(-1)
{
    
}

GeosTestCanvas::~GeosTestCanvas()
{

}

void GeosTestCanvas::setGeomParameters( const float buffer1,
                                        const float buffer2,
                                        const float buffer3)
{
    if( this->buffer1 == buffer1 &&
        this->buffer2 == buffer2 &&
        this->buffer3 == buffer3) { return ;}

    this->buffer1 = buffer1;
    this->buffer2 = buffer2;
    this->buffer3 = buffer3;

    geoms.clear();

    Geometry::Ptr shape = makeBox(-0.5,-0.5,0.5,0.5);

    Geometry::Ptr inside = unionPolygons({  makeBox(-0.1,-0.1,0.1,0.1),
                                            makeBox(-0.05,-0.6,0.05,0.6),
                                            makeBox(-0.6,-0.05,0.6,0.05)});

    shape = Geometry::Ptr(shape->buffer(buffer1));

    inside = Geometry::Ptr(inside->buffer(buffer2));

    shape = Geometry::Ptr(shape->difference(inside.get()));

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    Renderable * geom1 = Renderable::create(shape, trans);
     
    //geom1->setFillColor(vec4(0.3,0.3,0,1));
        
    //geom1->setOutlineColor(vec4(1,0,0,1));

    addRenderiable(geom1);

    geoms.push_back(unique_ptr<Renderable>(geom1));

    for(const LineString * ring : getExternalRings(shape))
    {
        Geometry::Ptr buffered(ring->buffer(buffer3));

        Geometry::Ptr buffered2(ring->buffer(buffer3 + 0.15));

        buffered = Geometry::Ptr(buffered->difference(inside.get()));

        for(const LineString * ring : getExternalRings(buffered))
        {   
            Renderable * geom = Renderable::create(ring, trans);

            //geom->setOutlineColor(vec4(0,1,0,1));
            geom->setOutlineColor(1);

            addRenderiable(geom);

            geoms.push_back(unique_ptr<Renderable>(geom));
        }

        /*
        for(const LineString * ring : getExternalRings(buffered2))
        {   
            Geometry * gg = ring->difference(inside.get());

            //dmess("gg " << gg->getGeometryType());

            Renderable * geom = Renderable::create(gg, trans);
            //Renderable * geom = RenderableLineString::create(ring, trans);
            //Renderable * geom = RenderableLineString::create(dynamic_cast<const LineString *>(gg), trans);
            if(geom)
            {
                geom->setOutlineColor(vec4(0,1,0,1));

                addRenderiable(geom);

                geoms.push_back(unique_ptr<Renderable>(geom));
            }
        }
        //*/
    }
    
}