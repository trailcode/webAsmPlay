﻿/**
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
  \copyright 2019
*/
#pragma once

#include <webAsmPlay/Types.h>

namespace geos
{
	namespace geom
	{
		class Polygon;
		class LineString;
	}
}

namespace boostGeom
{
	Polygon convert(const geos::geom::Polygon * poly);

	LineString convert(const geos::geom::LineString * lineString);

	geos::geom::LineString * toGeos(const LineString & ls);

	MultiPolygon unionPolygons(const MultiPolygon & polys);

	Polygon makePolygonBox(const glm::dvec2 & min, const glm::dvec2 & max);

	Polygon makeTriangle(const glm::dvec2 & A, const glm::dvec2 & B, const glm::dvec2 & C);

	Polygon makeCircle(const glm::dvec2 & pos, const double radius, const size_t pointsPerCircle = 10);

	Polygon toPolygon(const boostGeom::Box & b);

	Point toPoint(const glm::dvec2& P);
	
	template<typename GeomType>
	Point getCentroid(const GeomType & g)
	{
		Point center;

		boost::geometry::centroid(g, center);

		return center;
	}

	std::vector<boostGeom::Box> quadBox(const boostGeom::Box & b);

	void quadBox(const boostGeom::Box & b, std::vector<boostGeom::Box> & out);

	void subdevideBox(const boostGeom::Box & b, const size_t dim, const std::vector<boostGeom::Box> & out);

	void subdevideBox(const boostGeom::Box & b, const size_t dimX, const size_t dimY, const std::vector<boostGeom::Box> & out);
}

inline glm::dvec2 __(const boostGeom::Point & point) { return { point.x(), point.y() } ;}
