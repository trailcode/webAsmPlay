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

Polygon boostGeom::buffer(const dvec2 & pos, const double radius)
{
    const int points_per_circle = 10;

    boost::geometry::strategy::buffer::distance_symmetric<CoordinateType> distance_strategy(radius);
    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
    boost::geometry::strategy::buffer::end_round end_strategy(points_per_circle);
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
    boost::geometry::strategy::buffer::side_straight side_strategy;

	Point p(pos.x, pos.y);

	boost::geometry::model::multi_polygon<Polygon> result;

	boost::geometry::buffer(p, result, distance_strategy, side_strategy, join_strategy, end_strategy, circle_strategy);

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

	ret[0] = {	{min_x,			max_y		},		center					};
	ret[1] = {	{center.x(),	max_y		},	{	max_x,		center.y()	}};
	ret[2] = {	{min_x,			center.y()	},	{	center.x(), min_y		}};
	ret[3] = {	 center,						{	max_x,		max_y		}};

	return ret;
}

void boostGeom::quadBox(const Box & b, vector<Box> & out)
{
	const auto center = getCentroid(b);

	const auto min_x = get<min_corner, 0>(b);
	const auto min_y = get<min_corner, 1>(b);
	const auto max_x = get<max_corner, 0>(b);
	const auto max_y = get<max_corner, 1>(b);

	out.emplace_back(Box{	{min_x,			max_y		},		center					});
	out.emplace_back(Box{	{center.x(),	max_y		},	{	max_x,		center.y()	}});
	out.emplace_back(Box{	{min_x,			center.y()	},	{	center.x(), min_y		}});
	out.emplace_back(Box{	 center,						{	max_x,		max_y		}});
}

void boostGeom::subdevideBox(const Box & b, const size_t dim, const vector<Box> & out)
{

}

void boostGeom::subdevideBox(const Box & b, const size_t dimX, const size_t dimY, const vector<Box> & out)
{

}
