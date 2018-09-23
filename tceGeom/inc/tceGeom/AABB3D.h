/***************************************************************************
 *   Copyright (C) 2006 by Matthew Tang                                    *
 *   trailcode@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _T_BOUNDING_BOX_H__
#define _T_BOUNDING_BOX_H__

#include <ostream>
#include <tceGeom/vec3.h>
#include <tceGeom/plane.h>

#ifndef BOGUS_RANGE
	#define BOGUS_RANGE 180000 // @@@ Move to myLimits?
#endif

namespace tce
{
	namespace geom
	{

		template <typename vectorType>
		struct AABB3D
		{
			typedef typename vectorType::valuetype valueType;

			vectorType min;
			vectorType max;

			AABB3D() : min( BOGUS_RANGE,  BOGUS_RANGE,  BOGUS_RANGE),
					   max(-BOGUS_RANGE, -BOGUS_RANGE, -BOGUS_RANGE) { ;}

			AABB3D(const vectorType & mins, const vectorType & maxs) : min(mins), max(maxs) {}

            AABB3D(const AABB3D & other) : min(other.min), max(other.max) {}

			void reset()
			{
				min[0] =  BOGUS_RANGE;
				min[1] =  BOGUS_RANGE;
				min[2] =  BOGUS_RANGE;

				max[0] = -BOGUS_RANGE;
				max[1] = -BOGUS_RANGE;
				max[2] = -BOGUS_RANGE;
			}

            bool operator==(const AABB3D & other) const { return min == other.min && max == other.max ;}

			valueType  getLength() const { return vectorType::Length(max - min) ;}

			vectorType getExtent() const { return (max - min) * ((valueType)0.5) ;}

			vectorType getCenter() const { return (max + min) * ((valueType)0.5) ;}
			
			vectorType getCentroid() const { return getCenter() ;}
			
			void expand(const vectorType & amount)
			{
				min -= amount;
				max += amount;
			}

			bool intersects(const AABB3D & bbox, const valueType tolerance)
			{
				return
				    ((min.x - tolerance) <= (bbox.max.x + tolerance)) &&
				    ((min.y - tolerance) <= (bbox.max.y + tolerance)) &&
				    ((min.z - tolerance) <= (bbox.max.z + tolerance)) &&
				    ((max.x + tolerance) >= (bbox.min.x - tolerance)) &&
				    ((max.y + tolerance) >= (bbox.min.y - tolerance)) &&
				    ((max.z + tolerance) >= (bbox.min.z - tolerance));
			}

			vectorType addInternalPoint(const vectorType & point)
			{
				if(point.x > max.x) { max.x = point.x ;}
				if(point.y > max.y) { max.y = point.y ;}
				if(point.z > max.z) { max.z = point.z ;}

				if(point.x < min.x) { min.x = point.x ;}
				if(point.y < min.y) { min.y = point.y ;}
				if(point.z < min.z) { min.z = point.z ;}
				
				return point;
			}
			
			vectorType grow(const vectorType & point) { return addInternalPoint(point) ;}

			Plane3<valueType> get1stPlane() const { return get1sPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get1stPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x - extent.x, cp.y - extent.y, cp.z + extent.z),
				           vectorType(cp.x - extent.x, cp.y - extent.y, cp.z - extent.z),
				           vectorType(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z)
				       );
			}

			Plane3<valueType> get2ndPlane() const { return get2ndPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get2ndPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
				           vectorType(cp.x + extent.x, cp.y - extent.y, cp.z - extent.z),
				           vectorType(cp.x - extent.x, cp.y - extent.y, cp.z - extent.z)
				       );
			}

			Plane3<valueType> get3ndPlane() const { return get3ndPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get3ndPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x + extent.x, cp.y - extent.y, cp.z - extent.z),
				           vectorType(cp.x + extent.x, cp.y + extent.y, cp.z - extent.z),
				           vectorType(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z)
				       );
			}

			Plane3<valueType> get4thPlane() const { return get4thPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get4thPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x + extent.x, cp.y + extent.y, cp.z - extent.z),
				           vectorType(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
				           vectorType(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z)
				       );
			}

			Plane3<valueType> get5thPlane() const { return get5thPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get5thPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z),
				           vectorType(cp.x - extent.x, cp.y + extent.y, cp.z + extent.z),
				           vectorType(cp.x - extent.x, cp.y + extent.y, cp.z - extent.z)
				       );
			}

			Plane3<valueType> get6thPlane() const { return get6thPlane(getCentroid(), getExtent()) ;}

			Plane3<valueType> get6thPlane(const vectorType & cp, const vectorType & extent) const
			{
				return Plane3<valueType>
				       (
				           vectorType(cp.x + extent.x, cp.y - extent.y, cp.z + extent.z),
				           vectorType(cp.x - extent.x, cp.y + extent.y, cp.z + extent.z),
				           vectorType(cp.x + extent.x, cp.y + extent.y, cp.z + extent.z)
				       );
			}

			void getBoundingPlanes(Plane3<valueType> * planes)
			{
				const vectorType extent = getExtent();
				const vectorType cp = getCentroid();

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
				const vectorType extent = getExtent();
				const vectorType cp = getCentroid();

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
				const vectorType extent = getExtent();
				const vectorType cp = getCentroid();

				if(planeMask[0]) { planes.push_back(get1stPlane(cp, extent)) ;}
				if(planeMask[1]) { planes.push_back(get2ndPlane(cp, extent)) ;}
				if(planeMask[2]) { planes.push_back(get3ndPlane(cp, extent)) ;}
				if(planeMask[3]) { planes.push_back(get4thPlane(cp, extent)) ;}
				if(planeMask[4]) { planes.push_back(get5thPlane(cp, extent)) ;}
				if(planeMask[5]) { planes.push_back(get6thPlane(cp, extent)) ;}
			}

            int classify(const Plane3<valueType> & plane) const
            {
                // Convert AABB to center-extents representation
                const vectorType c = getCentroid();

                const vectorType e = max - c; // Compute positive extents

                // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
                valueType r = e[0]*fabs(plane.normal[0]) + e[1]*fabs(plane.normal[1]) + e[2]*fabs(plane.normal[2]);

                // Compute distance of box center from plane
                const valueType s = plane.normal.dot(c) - plane.dist;

                // Intersection occurs when distance s falls within [-r,+r] interval
                //fabs(s) <= r;
                /*
                 s < -r Back
                 s > r front
                 */

                if(s < -r) { return SIDE_BACK ;}

                else if(s > r) { return SIDE_FRONT ;}

                return SIDE_ON;

                /*
                if (dot < -ON_EPSILON)      { return SIDE_FRONT ;}
                else if (dot > ON_EPSILON)  { return SIDE_BACK  ;}
                else                        { return SIDE_ON    ;}
                 */

                return 0;
            }
		};

		typedef AABB3D < Vec3f > AABB3Df;
		typedef AABB3D < Vec3d > AABB3Dd;
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const tce::geom::AABB3D<T> & box)  
{  
    os << "(min: " << box.min << " max: " << box.max << ')';
	
    return os;  
}

#endif // _T_BOUNDING_BOX_H__
