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
\copyright 2019
*/
#pragma once

#include <limits>
#include <webAsmPlay/geom/Plane.h>

class AABB3D
{
public:

	glm::dvec3 min;
	glm::dvec3 max;

	AABB3D() :	min( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max(),  std::numeric_limits<double>::max()),
				max(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()) { ;}

	AABB3D(const glm::dvec3 & min, const glm::dvec3 & max) : min(min), max(max) {}

	AABB3D(const AABB3D & other) : min(other.min), max(other.max) {}

	void reset()
	{
		min = glm::dvec3( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max(),  std::numeric_limits<double>::max());
		max = glm::dvec3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	}

	bool operator==(const AABB3D & other) const { return min == other.min && max == other.max ;}

	double  getLength() const { return glm::distance(max, min) ;}

	glm::dvec3 getExtent() const { return (max - min) * 0.5 ;}

	glm::dvec3 getCenter() const { return (max + min) * 0.5 ;}

	glm::dvec3 getCentroid() const { return getCenter() ;}

	void expand(const glm::dvec3 & amount)
	{
		min -= amount;
		max += amount;
	}

	bool intersects(const AABB3D & bbox, const double tolerance)
	{
		return
			((min.x - tolerance) <= (bbox.max.x + tolerance)) &&
			((min.y - tolerance) <= (bbox.max.y + tolerance)) &&
			((min.z - tolerance) <= (bbox.max.z + tolerance)) &&
			((max.x + tolerance) >= (bbox.min.x - tolerance)) &&
			((max.y + tolerance) >= (bbox.min.y - tolerance)) &&
			((max.z + tolerance) >= (bbox.min.z - tolerance));
	}

	glm::dvec3 addInternalPoint(const glm::dvec3 & point)
	{
		if(point.x > max.x) { max.x = point.x ;}
		if(point.y > max.y) { max.y = point.y ;}
		if(point.z > max.z) { max.z = point.z ;}

		if(point.x < min.x) { min.x = point.x ;}
		if(point.y < min.y) { min.y = point.y ;}
		if(point.z < min.z) { min.z = point.z ;}

		return point;
	}

	glm::dvec3 grow(const glm::dvec3 & point) { return addInternalPoint(point) ;}

	Plane get1stPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x - extent.x, cp.y - extent.y, cp.z + extent.z),
						glm::dvec3(cp.x - extent.x, cp.y - extent.y, cp.z - extent.z),
						glm::dvec3(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z));
	}

	Plane get1stPlane() const { return get1stPlane(getCentroid(), getExtent()) ;}

	Plane get2ndPlane() const { return get2ndPlane(getCentroid(), getExtent()) ;}

	Plane get2ndPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
						glm::dvec3(cp.x + extent.x, cp.y - extent.y, cp.z - extent.z),
						glm::dvec3(cp.x - extent.x, cp.y - extent.y, cp.z - extent.z));
	}

	Plane get3ndPlane() const { return get3ndPlane(getCentroid(), getExtent()) ;}

	Plane get3ndPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x + extent.x, cp.y - extent.y, cp.z - extent.z),
						glm::dvec3(cp.x + extent.x, cp.y + extent.y, cp.z - extent.z),
						glm::dvec3(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z));
	}

	Plane get4thPlane() const { return get4thPlane(getCentroid(), getExtent()) ;}

	Plane get4thPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x + extent.x, cp.y + extent.y, cp.z - extent.z),
						glm::dvec3(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
						glm::dvec3(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z));
	}

	Plane get5thPlane() const { return get5thPlane(getCentroid(), getExtent()) ;}

	Plane get5thPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z),
						glm::dvec3(cp.x - extent.x, cp.y + extent.y, cp.z + extent.z),
						glm::dvec3(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z));
	}

	Plane get6thPlane() const { return get6thPlane(getCentroid(), getExtent()) ;}

	Plane get6thPlane(const glm::dvec3 & cp, const glm::dvec3 & extent) const
	{
		return Plane(	glm::dvec3(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
						glm::dvec3(cp.x - extent.x, cp.y + extent.y, cp.z + extent.z),
						glm::dvec3(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z));
	}

	void getBoundingPlanes(Plane * planes)
	{
		const glm::dvec3 extent = getExtent();
		const glm::dvec3 cp		= getCentroid();

		planes[0] = get1stPlane(cp, extent);
		planes[1] = get2ndPlane(cp, extent);
		planes[2] = get3ndPlane(cp, extent);
		planes[3] = get4thPlane(cp, extent);
		planes[4] = get5thPlane(cp, extent);
		planes[5] = get6thPlane(cp, extent);
	}

	template <typename container>
	void getBoundingPlanes(container & planes)
	{
		const glm::dvec3 extent = getExtent();
		const glm::dvec3 cp		= getCentroid();

		planes.push_back(get1stPlane(cp, extent));
		planes.push_back(get2ndPlane(cp, extent));
		planes.push_back(get3ndPlane(cp, extent));
		planes.push_back(get4thPlane(cp, extent));
		planes.push_back(get5thPlane(cp, extent));
		planes.push_back(get6thPlane(cp, extent));
	}

	template <typename container>
	void getBoundingPlanes(container planes, const bool * planeMask)
	{
		const glm::dvec3 extent = getExtent();
		const glm::dvec3 cp = getCentroid();

		if(planeMask[0]) { planes.push_back(get1stPlane(cp, extent)) ;}
		if(planeMask[1]) { planes.push_back(get2ndPlane(cp, extent)) ;}
		if(planeMask[2]) { planes.push_back(get3ndPlane(cp, extent)) ;}
		if(planeMask[3]) { planes.push_back(get4thPlane(cp, extent)) ;}
		if(planeMask[4]) { planes.push_back(get5thPlane(cp, extent)) ;}
		if(planeMask[5]) { planes.push_back(get6thPlane(cp, extent)) ;}
	}

	int classify(const Plane & plane) const
	{
		// Convert AABB to center-extents representation
		const glm::dvec3 c = getCentroid();

		const glm::dvec3 e = max - c; // Compute positive extents

									  // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		double r = e[0]*fabs(plane.normal[0]) + e[1]*fabs(plane.normal[1]) + e[2]*fabs(plane.normal[2]);

		// Compute distance of box center from plane
		const double s = glm::dot(plane.normal, c) - plane.dist;

		// Intersection occurs when distance s falls within [-r,+r] interval
		//fabs(s) <= r;
		/*
		s < -r Back
		s > r front
		*/

		if(s < -r) { return Plane::SIDE_BACK ;}

		else if(s > r) { return Plane::SIDE_FRONT ;}

		return Plane::SIDE_ON;

		/*
		if (dot < -ON_EPSILON)      { return SIDE_FRONT ;}
		else if (dot > ON_EPSILON)  { return SIDE_BACK  ;}
		else                        { return SIDE_ON    ;}
		*/

		return 0;
	}
};

