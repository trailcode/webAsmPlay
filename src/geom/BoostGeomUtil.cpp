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
	return toPolygon(Box(Point(min.x, min.y), Point(max.x, max.y)));
}

Polygon boostGeom::toPolygon(const Box & b)
{
	Polygon bx;

	boost::geometry::convert(b, bx);

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

Polygon boostGeom::makeCircle(const dvec2 & pos, const double radius, const size_t pointsPerCircle)
{
	using namespace boost::geometry::strategy::buffer;

	MultiPolygon result;

	boost::geometry::buffer(Point{pos.x, pos.y},
							result,
							distance_symmetric<CoordinateType>	(radius),
							side_straight(),
							join_round							(pointsPerCircle),
							end_round							(pointsPerCircle),
							point_circle						(pointsPerCircle));

	if(result.empty()) { return Polygon() ;}

	return result[0];
}

vector<Box> boostGeom::quadBox(const Box & b)
{
	vector<Box> ret(4);

	const auto center = getCentroid(b);

	const auto min_x = get<min_corner, 0>(b);
	const auto min_y = get<min_corner, 1>(b);
	const auto max_x = get<max_corner, 0>(b);
	const auto max_y = get<max_corner, 1>(b);
	
	/*
	.     max
	min   .

	min_x,max_y     center,max_y     max_x,max_y
	min_x,center    center,center    max_x,center
	min_x,min_y     center,min_y     max_x,min_y

	*/
	ret[0] = {	{min_x,			center.y()	},	{	center.x(), max_y		}};
	ret[1] = {	 center,						{	max_x,		max_y		}};
	ret[2] = {	{min_x,			min_y		},		center					};
	ret[3] = {	{center.x(),	min_y},			{	max_x,		center.y()	}};

	return ret;
}

void boostGeom::quadBox(const Box & b, vector<Box> & out)
{
	const auto center = getCentroid(b);

	const auto min_x = get<min_corner, 0>(b);
	const auto min_y = get<min_corner, 1>(b);
	const auto max_x = get<max_corner, 0>(b);
	const auto max_y = get<max_corner, 1>(b);

	out.emplace_back(Box{	{min_x,			center.y()	},	{	center.x(), max_y		}});
	out.emplace_back(Box{	 center,						{	max_x,		max_y		}});
	out.emplace_back(Box{	{min_x,			min_y		},		center					});
	out.emplace_back(Box{	{center.x(),	min_y},			{	max_x,		center.y()	}});
}

void boostGeom::subdevideBox(const Box & b, const size_t dim, const vector<Box> & out)
{

}

void boostGeom::subdevideBox(const Box & b, const size_t dimX, const size_t dimY, const vector<Box> & out)
{

}

Point boostGeom::toPoint(const dvec2& P) { return Point(P.x, P.y) ;}