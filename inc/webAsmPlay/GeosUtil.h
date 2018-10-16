#ifndef __WEB_ASM_PLAY_GEOS_UTIL_H__
#define __WEB_ASM_PLAY_GEOS_UTIL_H__

#include <vector>
#include <initializer_list>
#include <geos/geom/Geometry.h>

namespace geos
{
    namespace geom
    {
        class Geometry;
        class Polygon;
    }
}

#define _SCOPED_GEOS_GEOMETRY_COMBINE1(X, Y) X##Y
#define _SCOPED_GEOS_GEOMETRY_COMBINE(X, Y) _SCOPED_GEOS_GEOMETRY_COMBINE1(X, Y)

#define scopedGeosGeometry(geom) geom; _ScopedGeosGeometry _SCOPED_GEOS_GEOMETRY_COMBINE(scopedGeosGeom, __LINE__)(geom);

class _ScopedGeosGeometry
{
public:

    _ScopedGeosGeometry(geos::geom::Geometry * geom);
    
    ~_ScopedGeosGeometry();
    
private:

    geos::geom::Geometry * geom;
};

namespace geosUtil
{
    geos::geom::Geometry::Ptr makeBox(const double xmin, const double ymin, const double xmax, const double ymax);
    
    geos::geom::Geometry::Ptr unionPolygons(const std::initializer_list<geos::geom::Geometry::Ptr> & polys);
};

#endif