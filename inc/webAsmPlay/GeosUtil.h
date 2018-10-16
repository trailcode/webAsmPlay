#ifndef __WEB_ASM_PLAY_GEOS_UTIL_H__
#define __WEB_ASM_PLAY_GEOS_UTIL_H__

#include <initializer_list>

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

class GeosUtil
{
public:

    static geos::geom::Polygon * makeBox(const double xmin, const double ymin, const double xmax, const double ymax);
    
    static geos::geom::Geometry * unionPolygonsOwned(const std::initializer_list<geos::geom::Polygon *> & polys);

private:

    GeosUtil() {}
    ~GeosUtil() {}
};

/*
geos::geom::Geometry * unionGeoms     (const std::initializer_list<const geos::geom::Geometry *> & geoms);
geos::geom::Geometry * unionGeomsOwned(const std::initializer_list<geos::geom::Geometry *>       & geoms);
*/



#endif