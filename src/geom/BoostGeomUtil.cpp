#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>

using namespace boostGeom;

Polygon boostGeom::convert(const geos::geom::Polygon * poly)
{
	Polygon ret;

	for(const auto & P : *poly->getExteriorRing()->getCoordinatesRO()->toVector()) { ret.outer().push_back({ P.x, P.y }) ;}

	dmessError("Implement innors!");

	return ret;
}