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
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/canvas/GeosTestCanvas.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

//using bg = boost::geometry;

#include <glm/gtc/random.hpp>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>

GeosTestCanvas::GeosTestCanvas()
{

}

GeosTestCanvas::~GeosTestCanvas()
{

}

namespace
{
	vector<Geometry::Ptr> doGeosTest1(	const float buffer1,
										const float buffer2,
										const float buffer3)
	{
		vector<Geometry::Ptr> ret;

		auto shape = makeBox({-0.5,-0.5}, {0.5,0.5});

		auto inside = unionPolygons({	makeBox({-0.1 ,-0.1 }, {0.1	,0.1}),
										makeBox({-0.05,-0.6 }, {0.05,0.6}),
										makeBox({-0.6 ,-0.05}, {0.6	,0.05})});

		shape = Geometry::Ptr(shape->buffer(buffer1));

		ret.push_back(Geometry::Ptr(inside->buffer(buffer2)));

		ret.push_back(Geometry::Ptr(shape->difference(inside.get())));

		for(const LineString * ring : getExternalRings(shape))
		{
			Geometry::Ptr buffered(ring->buffer(buffer3));

			Geometry::Ptr buffered2(ring->buffer(buffer3 + 0.15));

			buffered = Geometry::Ptr(buffered->difference(inside.get()));

			for(const auto ring : getExternalRings(buffered)) { ret.push_back(Geometry::Ptr(ring->clone())) ;}
		}

		return ret;
	}

	// #define BOOST_GEOMETRY_DEBUG_HAS_SELF_INTERSECTIONS

	void boostMakeBox(const dvec2 & min, const dvec2 & max)
	{
		
	}

	boostGeom::MultiPolygon doBoostGeomTest1(const float buffer1,
	//boostGeom::Polygon doBoostGeomTest1(const float buffer1,
											const float buffer2,
											const float buffer3)
	{
		
		using namespace boost::geometry;
		using namespace boostGeom;
		using boostGeom::CoordinateType;
		using boostGeom::Point;
		using boostGeom::Box;
		using boostGeom::Polygon;
		using boostGeom::MultiPolygon;
		
		const double buffer_distance = buffer1;
		const int points_per_circle = 36;

		typedef strategy::buffer::distance_symmetric<CoordinateType> DistanceSymmetric;

		strategy::buffer::join_round join_strategy(points_per_circle);
		strategy::buffer::end_round end_strategy(points_per_circle);
		strategy::buffer::point_circle circle_strategy(points_per_circle);
		strategy::buffer::side_straight side_strategy;

		auto startShape = makePolygonBox({-0.5,-0.5}, {0.5,0.5});

		MultiPolygon shape;
		MultiPolygon insideBuffer;
		MultiPolygon theDiff;

		auto inside = unionPolygons({	makePolygonBox({-0.1 ,-0.1 }, {0.1	,0.1}),
										makePolygonBox({-0.05,-0.6 }, {0.05,0.6}),
										makePolygonBox({-0.6 ,-0.05}, {0.6	,0.05})});

		buffer(startShape,	shape,			DistanceSymmetric(buffer1), side_strategy, join_strategy, end_strategy, circle_strategy);
		buffer(inside,		insideBuffer, 	DistanceSymmetric(buffer2), side_strategy, join_strategy, end_strategy, circle_strategy);

		difference(shape, insideBuffer, theDiff);

		return theDiff;
	}

	pair<dvec2, dvec2> getCirclesTangetLine(const pair<dvec2, double> & circleA, const pair<dvec2, double> & circleB)
	{
		const auto & xA = get<0>(circleA)[0];
		const auto & yA = get<0>(circleA)[1];
		const auto & xB = get<0>(circleB)[0];
		const auto & yB = get<0>(circleB)[1];

		const auto RA = get<1>(circleA);
		const auto RB = get<1>(circleB);

		// Compute distance between circle centers
		const auto D = sqrt( (xB-xA)*(xB-xA) + (yB-yA)*(yB-yA) ); // Because they are on same axis duh!

		// New always First case : process external tangents

		// Compute the lenght of the tangents
		const auto L = sqrt(D*D - (RA-RB)*RA-RB);

		// Compute the parameters
		const auto R1 = sqrt(L*L+RB*RB);
		const auto Sigma1 = (1.0/4.0) * sqrt ( ( D+RA+R1 )*( D+RA-R1 )*( D-RA+R1 )*( -D+RA+R1 ) );
		const auto R2 = sqrt(L*L+RA*RA);
		const auto Sigma2= (1.0/4.0) * sqrt ( ( D+RB+R2 )*( D+RB-R2 )*( D-RB+R2 )*( -D+RB+R2 ) );

		// Compute the first tangent
		const auto x11= (xA+xB)/2.0 + (xB-xA)*(RA*RA-R1*R1)/(2*D*D) + 2.0*(yA-yB)*Sigma1/(D*D);
		const auto y11= (yA+yB)/2.0 + (yB-yA)*(RA*RA-R1*R1)/(2*D*D) - 2.0*(xA-xB)*Sigma1/(D*D);
		const auto x21= (xB+xA)/2.0 + (xA-xB)*(RB*RB-R2*R2)/(2*D*D) - 2.0*(yB-yA)*Sigma2/(D*D);
		const auto y21= (yB+yA)/2.0 + (yA-yB)*(RB*RB-R2*R2)/(2*D*D) + 2.0*(xB-xA)*Sigma2/(D*D);

		//return (Vector((x11, y11, 0)), Vector((x21, y21, 0)))
		return {{x11, y11}, {x21, y21}};
	}

	dvec2 computeCircleCenter(const dvec2 & A, const dvec2 & B, const dvec2 & C)
	{
		const auto yDelta_a = B.y - A.y;
		const auto xDelta_a = B.x - A.x;
		const auto yDelta_b = C.y - B.y;
		const auto xDelta_b = C.x - B.x;

		dvec2 center;

		const auto aSlope = yDelta_a/xDelta_a;
		const auto bSlope = yDelta_b/xDelta_b;  

		center.x = (aSlope*bSlope*(A.y - C.y) + bSlope*(A.x + B.x) - aSlope*(B.x+C.x) )/(2* (bSlope-aSlope) );
		center.y = -1*(center.x - (A.x+B.x)/2)/aSlope +  (A.y+B.y)/2;

		return center;
	}

	bool circle2PtRad(const dvec2 & p1, const dvec2 & p2, const double radius, dvec2 & c)
	{
		// From: https://stackoverflow.com/questions/42977182/how-to-find-centers-of-two-circle-when-their-intersecting-points-and-correspondi?rq=1
		const auto d2 = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);

		const auto det = radius * radius / d2 - 0.25;

		if(det < 0.0) { return false ;}

		const auto h = sqrt(det);

		c.x = (p1.x + p2.x) * 0.5 + (p1.y - p2.y) * h;
		c.y = (p1.y + p2.y) * 0.5 + (p2.x - p1.x) * h;

		return true;
	}

	void add(boostGeom::MultiPolygon & ret, boostGeom::MultiPolygon & in)
	{
		for(const auto & i : in) { ret.push_back(i) ;}
	}

	tuple<bool, double, double, boostGeom::Polygon, dvec2, dvec2, dvec2> tryParams(const boostGeom::Polygon & circleA, const boostGeom::Polygon & circleB, const float radiusA, const float radiusB, const float buffer3, const float buffer4, const float buffer5)
	{
		const auto [v1a, v2a] = getCirclesTangetLine({{0,0}, radiusA}, {{2 + buffer4, 0}, radiusB});
		const auto [v1b, v2b] = getCirclesTangetLine({{0 + buffer5, 0}, radiusA}, {{2, 0}, radiusB});

		const auto v1 = v1a;
		const auto v2 = v2b;

		using namespace boost::geometry;
		using boostGeom::MultiPolygon;
		using boostGeom::CoordinateType;

		typedef strategy::buffer::distance_symmetric<CoordinateType> DistanceSymmetric;
		strategy::buffer::side_straight side_strategy;
		strategy::buffer::join_round join_strategy(360);
		strategy::buffer::end_round end_strategy(360);
		strategy::buffer::point_circle point_strategy(360);

		dvec2 c;

		if(!circle2PtRad(v1, v2, buffer3, c))
		{
			return {false, 0, 0, boostGeom::Polygon(), v1, v2, c};
		}

		//MultiPolygon result8;

		//boost::geometry::buffer(boostGeom::Point{c.x, c.y}, result8, DistanceSymmetric(glm::distance(c, v1)), side_strategy, join_strategy, end_strategy, point_strategy); 

		//auto curveCircle = boostGeom::makeCircle(c, glm::distance(c, v1), 360);
		auto curveCircle = boostGeom::makeCircle(c, buffer3, 360);
		
		dmess("c " << c.x << "," << c.y << " r " << buffer3);

		//if(result8.empty())
		if(boost::geometry::is_empty(curveCircle))
		{
			return {false, 0, 0, boostGeom::Polygon(), v1, v2, c};
		}

		//auto curveCircle = result8[0];

		//auto curveCircle = boostGeom::makeCircle(c, glm::distance(c, v1), 360);

		MultiPolygon i1;
		MultiPolygon i2;

		intersection(circleA, curveCircle, i1);
		intersection(circleB, curveCircle, i2);

		const double a1 = boost::geometry::area(i1);
		const double a2 = boost::geometry::area(i2);

		return { true, a1, a2, curveCircle, v1, v2, c };
	}
	 

	boostGeom::MultiPolygon doBoostGeomTest2(	const float _buffer1,
												const float _buffer2,
												const float _buffer3,
												const float _buffer4,
												const float _buffer5)
	{
		//return boostGeom::MultiPolygon;

		float buffer1 = _buffer1;
		float buffer2 = _buffer2;
		float buffer3 = _buffer3;
		float buffer4 = _buffer4;
		float buffer5 = _buffer5;

		using namespace boost::geometry;
		using namespace boostGeom;
		using boostGeom::CoordinateType;
		using boostGeom::Point;
		using boostGeom::Box;
		using boostGeom::Polygon;
		using boostGeom::MultiPolygon;
		
		const double buffer_distance = buffer1;
		const int points_per_circle = 360;

		typedef strategy::buffer::distance_symmetric<CoordinateType> DistanceSymmetric;


		strategy::buffer::join_round	join_strategy(points_per_circle);
		strategy::buffer::end_round		end_strategy(points_per_circle);
		strategy::buffer::point_circle	circle_strategy(points_per_circle);
		strategy::buffer::side_straight side_strategy;

		//boost::geometry::model::multi_point<Point> mp = {{0,0}, {2,0}};
		//boostGeom::Point pA;

		//MultiPolygon result;
		MultiPolygon result1;
		MultiPolygon result2;
		MultiPolygon result8;

		// Declare the point_circle strategy
		strategy::buffer::point_circle point_strategy(360);

		buffer(Point{0,0}, result1, DistanceSymmetric(buffer1), side_strategy, join_strategy, end_strategy, point_strategy);
		buffer(Point{2,0}, result2, DistanceSymmetric(buffer2), side_strategy, join_strategy, end_strategy, point_strategy);

		MultiPolygon ret;
		
		DeferredRenderable::addLine({-3,0}, {3,0}, {1,0,0,1}, DEFER_FEATURES);

		auto [success, a1, a2, curveCircleTop, v1, v2, curveCircleCenter] = tryParams(result1[0], result2[0], buffer1, buffer2, buffer3, buffer4, buffer5);

		double delta1 = 0.5;
		double delta2 = 0.5;

		double deltaDelta = 0.5;

		double prevA1 = a1;
		double prevA2 = a2;

		for(size_t i = 0; i < 256; ++i)
		{
			if(prevA1 == 0.0 && prevA2 == 0.0)
			{
				dmess("Got it! " << i << " delta1 " << delta1 << " delta2 " << delta2 << " buffer4 " << buffer4 << " buffer5 " << buffer5);

				//break;

				goto gotIt;
			}

			if(a1 > a2)
			{
				buffer4 += delta1;

				tie(success, a1, a2, curveCircleTop, v1, v2, curveCircleCenter) = tryParams(result1[0], result2[0], buffer1, buffer2, buffer3, buffer4, buffer5);
					
				if(!success) { buffer4 -= delta1 ;}

				if(a1 > prevA1 || !success) { delta1 = -delta1 * deltaDelta ;}
			}
			else
			{
				buffer5 += delta2;

				tie(success, a1, a2, curveCircleTop, v1, v2, curveCircleCenter) = tryParams(result1[0], result2[0], buffer1, buffer2, buffer3, buffer4, buffer5);
					
				if(!success) { buffer5 -= delta2 ;}

				if(a2 > prevA2 || !success) { delta2 = -delta2 * deltaDelta ;}
			}

			if(!success) { continue ;}

			prevA1 = a1;
			prevA2 = a2;
		}

		dmess("Did not get it! delta1 " << delta1 << " delta2 " << delta2 << " buffer4 " << buffer4 << " buffer5 " << buffer5 << " a1 " << a1 << " a2 " << a2);

		gotIt:

		Polygon stock{{{v1.x, v1.y}, {v2.x, v2.y}, {v2.x, -v2.y}, {v1.x, -v1.y}, {v1.x, v1.y}}};
		
		MultiPolygon curveCircleB;

		buffer(Point{curveCircleCenter.x, -curveCircleCenter.y}, curveCircleB, DistanceSymmetric(glm::distance(curveCircleCenter, v1)), side_strategy, join_strategy, end_strategy, point_strategy); 

		ret.push_back(stock);

		add(ret, result1);
		add(ret, result2);

		ret = unionPolygons(ret);

		MultiPolygon ret2;
		MultiPolygon ret3;

		//add(ret3, c);
		ret3.push_back(curveCircleTop);
		add(ret3, curveCircleB);

		difference(ret, ret3, ret2);
		
		add(ret2, curveCircleB);

		DeferredRenderable::addCrossHairs({v1, v2}, 0.1, {1,0.3,0,1}, DEFER_FEATURES);
		
		//return ret;
		return ret2;
		//return ret3;
	}
}

void GeosTestCanvas::setGeomParameters( const float buffer1,
                                        const float buffer2,
                                        const float buffer3,
										const float buffer4,
										const float buffer5)
{
	return;

    if( m_buffer1 == buffer1 &&
        m_buffer2 == buffer2 &&
        m_buffer3 == buffer3 &&
		m_buffer4 == buffer4 &&
		m_buffer5 == buffer5) { return ;}

    m_buffer1 = buffer1;
    m_buffer2 = buffer2;
    m_buffer3 = buffer3;
	m_buffer4 = buffer4;
	m_buffer5 = buffer5;

    m_geoms.clear();

	const mat4 trans = scale(mat4(1.0), vec3(0.1, 0.1, 0.1));

	//for (const auto& geom : doGeosTest1(buffer1, buffer2, buffer3))
	//for (const auto& geom : doBoostGeomTest1(buffer1, buffer2, buffer3))
	{
		//auto geom = doBoostGeomTest1(buffer1, buffer2, buffer3);
		auto geom = doBoostGeomTest2(buffer1, buffer2, buffer3, buffer4, buffer5);
		
		//auto renderable = Renderable::create(geom, trans);

		auto renderable = RenderablePolygon::create(geom, trans);

		if(!renderable) { return ;}

		/*
		static ColorVertexShader * shader = nullptr;

		if(!shader) { shader = new ColorVertexShader() ;}
		*/

		static ColorShader * shader = nullptr;
		static ColorShader * shader2 = nullptr;

		if(!shader)
		{
			shader	= new ColorShader(Shader::s_defaultShouldRender);
			shader2 = new ColorShader(Shader::s_defaultShouldRender);
		}

		addRenderable(renderable->setShader			(shader)
								->setRenderFill		(true)
								->setRenderOutline	(true));

		m_geoms.push_back(unique_ptr<Renderable>(renderable));

		auto r3 = DeferredRenderable::createFromQueued(DEFER_FEATURES, trans);
		
		addRenderable(r3);

		m_geoms.push_back(unique_ptr<Renderable>(r3));
	}
}

void GeosTestCanvas::exportGeoJson() const
{
	geosUtil::writeGeoJsonFile("geomsOut.geojson", doGeosTest1(m_buffer1, m_buffer2, m_buffer3));
}
