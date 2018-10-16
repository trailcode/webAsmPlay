#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace geos::geom;
using namespace geos::operation::geounion;

Geometry::Ptr geosUtil::makeBox(const double xmin, const double ymin, const double xmax, const double ymax)
{
    const GeometryFactory * geomFact = GeometryFactory::getDefaultInstance();

    CoordinateSequence * temp = geomFact->getCoordinateSequenceFactory()->create((size_t) 0, 0);

    temp->add(Coordinate(xmin, ymin));
    temp->add(Coordinate(xmin, ymax));
    temp->add(Coordinate(xmax, ymax));
    temp->add(Coordinate(xmax, ymin));

    // Must close the linear ring or we will get an error:
    // "Points of LinearRing do not form a closed linestring"
    temp->add(Coordinate(xmin, ymin));

    LinearRing * shell = geomFact->createLinearRing(temp);

    // NULL in this case could instead be a collection of one or more holes
    // in the interior of the polygon
    return Geometry::Ptr(geomFact->createPolygon(shell, NULL));
}

Geometry::Ptr geosUtil::unionPolygons(const initializer_list<Geometry::Ptr> & polys)
{
    vector<Polygon *> _polys;

    for(const Geometry::Ptr & g : polys)
    {
        Polygon * p = dynamic_cast<Polygon *>(g.get());

        if(p) { _polys.push_back(p) ;}

        else { dmess("Warning not a polygon!") ;}
    }

    CascadedPolygonUnion unioner(&_polys);

    return Geometry::Ptr(unioner.Union());
}

_ScopedGeosGeometry::_ScopedGeosGeometry(Geometry * geom) : geom(geom) {}

_ScopedGeosGeometry::~_ScopedGeosGeometry()
{
    GeometryFactory::getDefaultInstance()->destroyGeometry(geom);
}

/*
Geometry * unionGeoms(const initializer_list<const Geometry *> & geoms)
{
    for(const Geometry * g;
    return NULL;
}

Geometry * unionGeomsOwned(const initializer_list<Geometry *> & geoms)
{
    return NULL;
}
*/





