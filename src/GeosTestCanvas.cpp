#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/Polygon.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/GeosTestCanvas.h>

using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

GeosTestCanvas::GeosTestCanvas() : geom1(NULL)
{
    
}

GeosTestCanvas::~GeosTestCanvas()
{

}

void GeosTestCanvas::setGeomParameters(const float buffer1, const float buffer2)
{
    delete geom1;

    Geometry::Ptr shape = makeBox(-0.5,-0.5,0.5,0.5);

    Geometry::Ptr inside = unionPolygons({  makeBox(-0.1,-0.1,0.1,0.1),
                                            makeBox(-0.05,-0.6,0.05,0.6),
                                            makeBox(-0.6,-0.05,0.6,0.05)});

    shape = Geometry::Ptr(shape->buffer(buffer1));

    inside = Geometry::Ptr(inside->buffer(buffer2));

    shape = Geometry::Ptr(shape->difference(inside.get()));

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    geom1 = Renderiable::create(shape.get(), trans);
    
    geom1->setFillColor(vec4(0.3,0.3,0,1));
        
    geom1->setOutlineColor(vec4(1,0,0,1));

    addRenderiable(geom1);
}