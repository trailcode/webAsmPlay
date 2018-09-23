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
 
#ifndef __T_PLANE_H__
#define __T_PLANE_H__

#define SIDE_ON         0
#define SIDE_FRONT      1
#define SIDE_BACK       2
#define SIDE_BOTH       3

#define ON_EPSILON  0.01

#include <list>
#include <vector>
#include <tceGeom/vec3.h>

namespace tce 
{
	namespace geom
	{
		/**
		* A plane or a 3D half space. Template parameter valueType is the number type to 
		* use. 
		*/
		template <typename valueType>
		struct Plane3
		{
			/**
			* Provides access to the normal data type
			*/
			typedef Vec3 < valueType > normalType;
		
			/**
			* Provides access to the point data type
			*/
			typedef Vec3 < valueType > pointType;
		
			/**
			* Provides access to the vector data type
			*/
			typedef Vec3 < valueType > vectorType;
		
			/**
			* Creates a plane defined by a normal and a distance from the origin (0,0,0)
			*/
			Plane3 (normalType _normal, valueType _dist) : normal(_normal), dist(_dist) { ;};
		
			/**
			* Creates a plane defined by any point on the plane and its normal
			*/
			Plane3 (pointType pointOnPlane, normalType NORMAL) : normal(NORMAL), dist(pointOnPlane.dot(normal)) { ;}
		
			/**
			* Creates a plane defined by three points. The order of point rotation, eg 
			* clockwise or counter clockwise defines what the front and back side of the 
			* plane will be. 
			*/
			Plane3 (const normalType & P1, const normalType & P2, const normalType & P3) : normal(), dist()
			{
		
				// Using the three point plane definition, construct the plane's normal
				normal.cross(P2 - P1, P3 - P1);
		
				normalType::Normalize(normal);
		
				// and the plane's distance from the origin.
				dist = normal.dot(P1);
			}

			Plane3(const valueType a, const valueType b, const valueType c, const valueType d)
			{
				// set the normal vector
				normal = vectorType(a,b,c);

				//compute the lenght of the vector
				const valueType l = normal.length();

				// normalize the vector
				normal.set(a/l,b/l,c/l);

				// and divide d by th length as well
				dist = d/l;
			}

			Plane3(const Plane3 & plane) : normal(plane.normal), dist(plane.dist) { ;}
				
			/**
			* Creates an uninitalized plane
			*/
			Plane3() : normal(), dist() {;};
		
			~Plane3() {;}
		
			/**
			* Sets this plane to be exactly like plane
			*/
			Plane3 & operator=(const Plane3 & plane)
			{
				dist = plane.dist;

				normal = plane.normal;

				return *this;
			}
		
			/**
			* Sets this plane to be the inverts of plane. That is both planes will divide 
			* a space in half exactly the same, just the normal of this plane will be 
			* facing in the opposite direction.  
			*/
			void setInverted(const Plane3 & plane)
			{
				dist = -plane.dist;
				normal = -plane.normal;
			}
		
			/**
			* The planes normal
			*/
			normalType normal;
		
			/**
			* The planes distance from the origin (0,0,0)
			*/
			valueType dist;
		
			/**
			* Figures out if the parameter point is lying on this plane, behind the 
			* plane, or in front of the plane. The corresponding values SIDE_ON, 
			* SIDE_BACK, or SIDE_FRONT is returned. 
			*/
			int classifyPoint (const pointType & point)
			{
				//valueType dot = point.dot(normal) - dist;
                valueType dot = normal.dot(point) + dist;
		
				if (dot < -ON_EPSILON)      { return SIDE_FRONT ;}
				else if (dot > ON_EPSILON)  { return SIDE_BACK  ;}
				else                        { return SIDE_ON    ;}
			}

			/**
			* Finds the shortest distance from the parameter point to this plane. If the 
			* point is in front of the plane then the returned distance will be negative, 
			* otherwise it will be positive. Use the fabs() to get the real distance. 
			*/
			const valueType getDistanceFromPlane(const pointType & point) const
			{
				return point.dot(normal) - dist;
			}
		
			/**
			* Finds a point on this plane where the line segment defined by the 
			* parameters lineStart and lineEnd. This point will be passed out on the 
			* parameter intersection. Also this intersect point lies on the line segment, 
			* so if a new line segment is imagined: lineStart to intersection.  The 
			* percent of this new line segment's length in comparison to the first line 
			* segment's length is passed out through the parameter percent. If no 
			* intersection exists between the first line segment and this plane, then the 
			* function returns false, otherwise true. 
			*/
			const bool getIntersect( const pointType & lineStart, const pointType & lineEnd,
									pointType & intersection, valueType & percent) const
			{
				vectorType direction;
		
				valueType lineLength;
		
				direction = lineEnd - lineStart;
		
				lineLength = direction.dot(-normal);
		
				if(fabs(lineLength) < 0.01) { return false ;}
		
				percent =  getDistanceFromPlane(lineStart) / lineLength;
		
				if(percent < 0.0f | percent > 1.0f) { return false ;}
		
				intersection = lineStart + (direction * percent);
		
				return true;
		
			}
		
			/**
			* Returns true if the parameter plane, is exactly facing in the same 
			* direction as this plane. That is their normals have to be equal. Otherwise 
			* returns false.  
			*/
			bool sameFacing ( const Plane3 & plane )
			{
				return normal.fuzzyCompare(plane.normal, 1e-5f);
			}
		
			/**
			* Returns true if the parameter plane is exactly the same as this plane. 
			* Otherwise returns false.
			*/
			bool samePlane ( const Plane3 & plane )
			{
				return (fabs(plane.dist - dist) < 1e-5f)
					&& sameFacing ( plane );
		
			}
		
			/**
			* Swaps the endianes of this data structure. If the data was in big endian 
			* before this function was called, then it is in little endian now and vise 
			* versa.
			*/
			void convertEndian()
			{
				normal.convertEndian();
		
				// @@@ Make this work for short, etc
				assert(sizeof(valueType) == sizeof(unsigned int));
		
				unsigned int tmp = *(reinterpret_cast<unsigned int*>(&dist));
				CONVERT_ENDIAN_UINT32(tmp);
		
				dist = *(reinterpret_cast<valueType*>(&tmp));
		
			}
		
			/**
			* Inverts this plane. That is this plane will divide a space in half exactly 
			* the same as it did before, just the normal of this plane will be facing in 
			* the opposite direction.   
			*/
			void invert()
			{
				dist   *= valueType(-1.0);
				normal *= valueType(-1.0);
			}

            void invertNormal()
            {
                normal *= valueType(-1.0);
            }

			/**
			* Used to get the type of the number values used in this plane, for example 
			* the number type for dist.
			*/
			typedef valueType valuetype;
		
		};
		
		typedef Plane3<float> Plane3f;
		typedef Plane3<double> Plane3d;
		typedef Plane3<int> Plane3i;
		
		/**
		* Template plane container wrapper, provides a constant and normal iterator 
		* to the container.
		*/
		template <typename Container>
		class planeContainer
		{
			public:
				
				planeContainer() : container() { ;}

				typedef typename Container::iterator iterator;

				typedef typename Container::const_iterator const_iterator;

				typedef typename Container::reverse_iterator reverse_iterator;	

				typedef typename Container::const_reverse_iterator const_reverse_iterator;

				typedef typename Container::size_type size_type;

				typedef typename Container::value_type value_type;

				inline iterator begin() { return container.begin() ;}

				inline const_iterator begin() const { return container.begin() ;}

				inline iterator end() { return container.end() ;}

				inline const_iterator end() const { return container.end() ;}

				inline reverse_iterator rbegin() { return container.rbegin() ;}

				inline const_reverse_iterator rbegin() const { return container.rbegin() ;}

				inline reverse_iterator rend() { return container.rend() ;}

				inline const_reverse_iterator rend() const { return container.rend() ;}

				inline bool empty() const { return container.empty() ;}

				inline size_type size() const { return container.size() ;}

				inline value_type & operator[] (const unsigned int i) { return container[i] ;}

				inline const value_type & operator[] (const unsigned int i) const { return container[i] ;}

				inline void push_back(const value_type plane) { container.push_back(plane) ;}

			private:

				Container container;
		};
		
		
		typedef planeContainer <std::list   < Plane3f > > planefList;
		typedef planeContainer <std::list   < Plane3d > > planedList;
		typedef planeContainer <std::list   < Plane3i > > planeiList;
		typedef planeContainer <std::vector < Plane3f > > planefVector;
		typedef planeContainer <std::vector < Plane3d > > planedVector;
		typedef planeContainer <std::vector < Plane3i > > planeiVector;
		
		/**
		* Template plane container wrapper, provides a constant and normal iterator 
		* to the container.
		*/
		template <typename Container>
		class BSPPlaneContainer
		{
			public:

				BSPPlaneContainer() : container() { ;}
	
				virtual ~BSPPlaneContainer() { ;}
	
				/**
				*  Provides access to the iterator of the container
				*/
				typedef typename Container::iterator iterator;
			
				/**
				* Provides access to the const_iterator of the container
				*/
				typedef typename Container::const_iterator const_iterator;

				typedef typename Container::reverse_iterator reverse_iterator;	

				typedef typename Container::const_reverse_iterator const_reverse_iterator;

				typedef typename Container::size_type size_type;

				typedef typename Container::value_type value_type;

				/**
				* stl container member functions
				*/

				inline iterator begin() { return container.begin() ;}

				inline const_iterator begin() const { return container.begin() ;}

				inline iterator end() { return container.end() ;}

				inline const_iterator end() const { return container.end() ;}

				inline reverse_iterator rbegin() { return container.rbegin() ;}

				inline const_reverse_iterator rbegin() const { return container.rbegin() ;}

				inline reverse_iterator rend() { return container.rend() ;}

				inline const_reverse_iterator rend() const { return container.rend() ;}

				inline bool empty() const { return container.empty() ;}

				inline size_type size() const { return container.size() ;}

				inline value_type & operator[] (const unsigned int i) { return container[i] ;}

				inline const value_type & operator[] (const unsigned int i) const { return container[i] ;}

				inline void push_back(const value_type plane) { container.push_back(plane) ;}

				/**
				* Returns the index of the plane in the container which is identical to 
				* the parameter plane if the plane is not found then -1 is returned. The 
				* container must implement stl iterator and be a container which 
				* contains planes of type Planef or a compatible type. 
				*/
				int getPlaneIndex(Plane3f & plane)
				{
					iterator i;
					
					int index = 0;
					
					for(i = this->begin(); i != this->end(); ++i)
					{
						if((**i).samePlane(plane)) { return index ;}
						
						index++;
					}
			
					return -1;
				}

			private:

				Container container;
		};
		
		typedef BSPPlaneContainer <std::vector < Plane3f * > > BSPPlane3fVector;
	}
}

#endif // __T_PLANE_H__
