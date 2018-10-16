#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/Polygon.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/GeosTestCanvas.h>

using namespace glm;
using namespace geos::geom;

GeosTestCanvas::GeosTestCanvas() : geom1(NULL)
{
    
}

GeosTestCanvas::~GeosTestCanvas()
{

}

void GeosTestCanvas::setGeomParameters(const float buffer1, const float buffer2)
{
    delete geom1;

    Geometry * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Geometry * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    Geometry * ppp = scopedGeosGeometry(GeosUtil::makeBox(-0.05,-0.6,0.05,0.6));

    Geometry * pppp = scopedGeosGeometry(GeosUtil::makeBox(-0.6,-0.05,0.6,0.05));

    Geometry * aa = GeosUtil::unionPolygonsOwned({  GeosUtil::makeBox(-0.1,-0.1,0.1,0.1),
                                    GeosUtil::makeBox(-0.05,-0.6,0.05,0.6),
                                    GeosUtil::makeBox(-0.6,-0.05,0.6,0.05)});

    p = scopedGeosGeometry(p->buffer(buffer1));

    /*
    p = scopedGeosGeometry(p->difference(pp));

    p = scopedGeosGeometry(p->difference(ppp));

    p = scopedGeosGeometry(p->difference(pppp));
    */

    dmess("aa " << aa->getGeometryType());

    //aa = aa->buffer(buffer2 + 0.3);
    aa = aa->buffer(buffer2);

    p = scopedGeosGeometry(p->difference(aa));

    if(!p) { dmess("!p"); return ;}

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    geom1 = Renderiable::create(p, trans);
    
    geom1->setFillColor(vec4(0.3,0.3,0,1));
        
    geom1->setOutlineColor(vec4(1,0,0,1));

    addRenderiable(geom1);
}