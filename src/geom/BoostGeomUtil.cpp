#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>

using namespace std;
using namespace glm;
using namespace boost::geometry;
using namespace boostGeom;

Polygon boostGeom::convert(const geos::geom::Polygon * poly)
{
	Polygon ret;

	for(const auto & P : *poly->getExteriorRing()->getCoordinatesRO()->toVector()) { ret.outer().push_back({ P.x, P.y }) ;}

	dmess("Implement innors!");

	return ret;
}

LineString boostGeom::convert(const geos::geom::LineString * lineString)
{
	LineString ret;

	for(const auto & P : *lineString->getCoordinatesRO()->toVector()) { ret.push_back({ P.x, P.y }) ;}

	return ret;
}

geos::geom::LineString * boostGeom::toGeos(const LineString & ls)
{
	using namespace geos::geom;

	auto coords = new vector<Coordinate>(ls.size());

	for(size_t i = 0; i < ls.size(); ++i) { (*coords)[i] = {ls[i].x(), ls[i].y()} ;}

	return GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(coords, 2));
}

MultiPolygon boostGeom::unionPolygons(const MultiPolygon & polys)
{
	MultiPolygon ret;

	for(const auto & poly : polys)
	{
		MultiPolygon tmp;

		union_(ret, poly, tmp);

		ret = tmp;
	}

	return ret;
}

Polygon boostGeom::makePolygonBox(const dvec2 & min, const dvec2 & max)
{
	Polygon bx;

	boost::geometry::convert(Box(Point(min.x, min.y), Point(max.x, max.y)), bx);

	return bx;
}

Polygon boostGeom::makeTriangle(const dvec2 & A, const dvec2 & B, const dvec2 & C)
{
	Polygon tri;

	tri.outer().push_back({A.x, A.y});
	tri.outer().push_back({B.x, B.y});
	tri.outer().push_back({C.x, C.y});
	tri.outer().push_back({A.x, A.y});

	return tri;
}