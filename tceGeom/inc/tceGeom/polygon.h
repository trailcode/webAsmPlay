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

#ifndef __T_POLYGON_H__
#define __T_POLYGON_H__

#include <math.h>
#include <memory>
#include <tceGeom/plane.h>
#include <tceGeom/AABB3D.h>

#define POLYS_DIFFERENT 0
#define POLYS_DIFF_NORMALS 1
#define POLYS_SAME 2

#ifndef BOGUS_RANGE
#define BOGUS_RANGE 18000
#endif

namespace tce
{
	namespace geom
	{

		/**
		 * Templated polygon structure definable by a vertex container vertexContainer, 
		 * and a plane type plane type. The vertex container must implement stl;s 
		 * const_iterator and iterator.
		 */
		template <typename vertexContainer, typename planeType>
		struct Polygon
		{
			/**
			 * Holds the vertices's for this polygon, they should be ordered in the 
			 * container in rotational manner related to the normal of the polygons plane. 
			 */
			vertexContainer & verts;

			/**
			 * Defines this polygon's vertex container type
			 */
			typedef vertexContainer vertexContainerType;

			/**
			 * Defines this polygons plane type
			 */
			typedef planeType planeTypeType;

			/**
			 * Defines a constant vertex iterator for this polygon.  
			 */
			typedef typename vertexContainer::const_iterator const_iterator;

			/**
			 * Defines a vertex iterator for this polygon.  
			 */
			typedef typename vertexContainer::iterator iterator;

			/**
			 * Defines the type of vertexes that make up this polygon
			 */
			typedef typename vertexContainer::vertType vertexType;

			/**
			 * Defines the numerical value type this polygon uses
			 */
			typedef typename vertexType::valuetype valueType;

			/**
			 * Creates a uninitialized polygon
			 */
			Polygon() : verts(*(new vertexContainer)), plane() { ;}

			/**
			 * Creates a polygon from a representative ordered collection of vertices's. Does not setup this polygon's plane.
			 */
			Polygon(vertexContainer & VERTS) : verts(VERTS), plane() { ;}

			/**
			 * Creates an extremely large polygon that lies on the parameter plane. See 
			 * BOGUS_RANGE for how big the polygon will be.
			 */
			Polygon(const Plane3 < valueType > & plane) : verts(*(new vertexContainer)), plane()
			{
				makeFromPlane(plane);

				this->plane = plane;
			}

			/**
			 * Creates a polygon that lines on the parameter plane, but is contained in 
			 * the axis aligned bounding box of parameter BB
			 */
			Polygon(const Plane3 < valueType > & plane, const AABB3D < vertexType > & BB)
					: verts(*(new vertexContainer))
			{
				makeFromPlane(plane, BB);

				this->plane = plane;
			}

			virtual ~Polygon()
			{
				// TODO What about cleaning up?
			}

			/**
			 * Sets this polygon to be exactly like the passed in polygon poly
			 */
			void set(Polygon * poly)
			{
				plane = poly->plane;
				verts.clear();

				const_iterator vert;

				for(vert = poly->verts.begin(); vert != poly->verts.end(); ++vert)
				{
					verts.push_back(*vert);
				}
			}

			/**
			 * The plane of this polygon
			 */
			planeType plane;

			/**
			 * Compairs this polygon to a polygon defined by cmpPolyVerts, and 
			 * cmpPolyPlane. Returns the int constant POLYS_DIFFERENT if the two are 
			 * completely different, POLYS_DIFF_NORMALS if their normals are different, 
			 * but their vertices's are the same, or returns POLYS_SAME if they are the 
			 * same.
			 */
			int compair (const vertexContainer & cmpPolyVerts, const planeType & cmpPolyPlane)
			{
				if(verts.size() != cmpPolyVerts.size())
				{
					return POLYS_DIFFERENT;
				}
				else
				{
					const_iterator vertA;

					const_iterator vertB;

					size_t count = 0;

					for(vertA = verts.begin(); vertA != verts.end(); ++vertA)
					{
						for(vertB = cmpPolyVerts.begin(); vertB != cmpPolyVerts.end(); ++vertB)
						{
							if((*vertA) == (*vertB))
							{
								count++;

								break;
							}
						}
					}

					if(count != verts.size()) {  return POLYS_DIFFERENT ;}

					if(fabs(plane.dist - cmpPolyPlane.dist) > 0.01)
					{
						return POLYS_DIFF_NORMALS;
					}

					for(int i = 0; i < 3; ++i)
					{
						if(fabs(plane.normal[i] - cmpPolyPlane.normal[i]) > 0.01)
						{

							return POLYS_DIFF_NORMALS;
						}
					}

					return POLYS_SAME;
				}
			}

			/**
			 * Compairs this polygon to the polygon cmpPoly. Returns the int constant 
			 * POLYS_DIFFERENT if the two are completely different, POLYS_DIFF_NORMALS if 
			 * their normals are different, but their vertices's are the same, or returns 
			 * POLYS_SAME if they are the same.
			 */
			int compair (const Polygon & cmpPoly) const
			{

				return compair(cmpPoly.verts, cmpPoly.plane);
			}

			/**
			 * Returns true if the parameter point is on this polygon. False otherwise.
			 */
			bool pointOnPolygon(const vertexType & point)
			{

				if(plane.classifyPoint (point) != SIDE_ON)
				{
					return false;
				}

				const_iterator vertA;
				const_iterator vertB;

				typename planeType::valuetype angSum = 0;

				for(vertA = verts.begin(); vertA != verts.end(); ++vertA)
				{
					if(point == (*vertA)) { return true ;}

					vertB = vertA;	vertB++;

					if(vertB == verts.end()) { vertB = verts.begin() ;}

					vertexType vec1 = (*vertA) - point;
					vertexType vec2 = (*vertB) - point;

					vertexType::Normalize ( vec1 );
					vertexType::Normalize ( vec2 );

					angSum += acos(vec1.dot(vec2));

				}

				valueType diff = angSum - (M_PI * 2);
				if((diff < -ON_EPSILON) | (diff > ON_EPSILON))
				{
					return false;
				}

				return true;
			}

			/**
			 * Returns true if the polygon cmpPoly is contained in this polygon. That is 
			 * it is a subset of this polygon, that is both are on a common plane and  
			 * none of cmpPolys vertices's are outside of this polygon. 
			 */
			bool isContained(const Polygon & cmpPoly)
			{

				const_iterator vert;
				for(vert = cmpPoly.verts.begin(); vert != cmpPoly.verts.end(); ++vert)
				{
					if(!pointOnPolygon(*vert))
					{
						return false;
					}
				}

				return true;
			}

			/**
			 * Returns a int constant SIDE_FRONT if this polygon is in front of the plane, 
			 * SIDE_BACK if it is to the planes back, SIDE_ON if it is on the plane, or 
			 * SIDE_BOTH if it is split by the plane. 
			 */
			int classifyPlane ( Plane3 < valueType > & plane ) const
			{
				const_iterator i;

				unsigned char ret = 0;

				for(i = verts.begin(); i != verts.end(); ++i)
				{
					ret |= plane.classifyPoint(*i);
					if(ret == SIDE_BOTH) { return ret ;}

				}
				return ret;
			}

			/**
			 * Splits this polygon by a plane. Returns the new polygon to the back 
			 * of the plane, the new polygon in front of the plane will not be 
			 * this polygon.
			 */
			Polygon *splitPolygon( Plane3 < valueType > & spliPlane )
			{
				return splitPolygon( spliPlane, plane );
			}

			/**
			 * Splits this polygon by a plane. Returns the new polygon to the back 
			 * of the plane, the new polygon in front of the plane will not be 
			 * this polygon. The parameter polyPlane is the plane of this polygon.
			 */
			Polygon *splitPolygon( Plane3 < valueType > & spliPlane, const Plane3 < valueType > & polyPlane)
			{
				Polygon *newPoly = new Polygon;
				return splitPolygon(spliPlane, polyPlane, newPoly);

			}

			/**
			 * Splits this polygon by a plane. The new polygon on the front side 
			 * of the split plane is passed out through the parameter frontPoly. The new 
			 * polygon to the back of the split plane is passed out through the parameter 
			 * backPoly. This polygon is not changed in anyway. 
			 */
			void splitPolygon(Plane3 < valueType > & spliPlane, Polygon * frontPoly, Polygon * backPoly)
			{
				frontPoly->set(this);
				frontPoly->splitPolygon(spliPlane, plane, backPoly);
			}

			/**
			 * Splits this polygon by a plane. The new polygon on the front side 
			 * of the split plane is passed out through the parameter frontPoly. The new 
			 * polygon to the back of the split plane is passed out through the parameter 
			 * backPoly. This polygon is not changed in anyway. The parameter polyPlane is 
			 * the plane of this polygon.
			 */
			void splitPolygon(Plane3 < valueType > & spliPlane, Plane3 < valueType > & polyPlane, Polygon * frontPoly, Polygon * backPoly)
			{
				frontPoly->set(this);
				frontPoly->splitPolygon(spliPlane, polyPlane, backPoly);
			}

			/**
			 * Splits this polygon by a split plane being spliPlane. The new polygon that 
			 * is on the front side of the split plane is returned. The new polygon to the 
			 * back of the split plane is passed out through the parameter newPoly. The 
			 * parameter polyPlane is the plane of this polygon.
			 * @@@ From gtkRadiant
			 */
			Polygon *splitPolygon( Plane3 < valueType > & spliPlane, const Plane3 < valueType > & polyPlane, Polygon * newPoly)
			{
				// 50 is used here, as we should never
				// really have more points on a portal than this.

				std::unique_ptr<vertexType[]> _FrontList(new vertexType[verts.size() + 10]);
                std::unique_ptr<vertexType[]> _BackList(new vertexType[verts.size() + 10]);

				vertexType * FrontList = _FrontList.get();
				vertexType * BackList = _BackList.get();

				int FrontCounter = 0;
				int BackCounter = 0;
                
                std::unique_ptr<int[]> _PointLocation(new int[verts.size() + 10]);

				int * PointLocation = _PointLocation.get();

				int InFront = 0, Behind = 0, OnPlane = 0;
				int CurrentVertex = 0, Location = 0;
                
                std::unique_ptr<vertexType[]> _VertexList(new vertexType[verts.size() + 10]);
				vertexType * VertexList = _VertexList.get();

				// Determine each points location relative to the plane.

				int i = 0;

				int NumberOfVertices = verts.size();

				const_iterator vert;

				for(vert = verts.begin(); vert != verts.end(); ++vert)
				{
					VertexList[i++] = *vert;
				}

				verts.clear();

				for (i = 0; i < NumberOfVertices; i++)
				{

					Location = spliPlane.classifyPoint(VertexList[i]);

					if (Location == SIDE_FRONT )
						InFront++;
					else if (Location == SIDE_BACK )
						Behind++;
					else
						OnPlane++;

					PointLocation[i] = Location;
				} // next Vertex

				// We set the vertices[0] location again at the end
				// of the array so that we don't have to check and loop later
				//PointLocation[Poly->numVertices] = PointLocation[0];

				if (!InFront)
				{

					for(i = 0; i < NumberOfVertices; ++i)
					{
						newPoly->verts.push_back(VertexList[i]);

					}

				}

				if (!Behind)
				{

					for(i = 0; i < NumberOfVertices; ++i)
					{
						verts.push_back(VertexList[i]);
						delete newPoly;
						newPoly = NULL;
					}
				}

				if (InFront && Behind)
				{

					for ( i = 0; i < NumberOfVertices; i++)
					{
						// Store Current vertex remembering to MOD with number of vertices.
						CurrentVertex = (i+1) % NumberOfVertices;

						if (PointLocation[i] == SIDE_ON )
						{
							FrontList[FrontCounter] = VertexList[i];
							FrontCounter++;
							BackList[BackCounter] = VertexList[i];
							BackCounter++;
							continue; // Skip to next vertex
						}
						if (PointLocation[i] == SIDE_FRONT )
						{
							FrontList[FrontCounter] = VertexList[i];
							FrontCounter++;
						}
						else
						{
							BackList[BackCounter] = VertexList[i];
							BackCounter++;
						}

						// If the next vertex is not causing us to span the plane then continue
						if (PointLocation[CurrentVertex] == SIDE_ON || PointLocation[CurrentVertex] == PointLocation[i]) continue;

						// Otherwise create the new vertex
						vertexType IntersectPoint;
						valueType percent;

						spliPlane.getIntersect(VertexList[i], VertexList[CurrentVertex], IntersectPoint, percent);

						// create new vertex and calculate new texture coordinate
						vertexType copy;

						//float deltax	= Poly->vertices[CurrentVertex].tu - Poly->vertices[i].tu;
						//float deltay	= Poly->vertices[CurrentVertex].tv - Poly->vertices[i].tv;
						//float texx		= Poly->vertices[i].tu + ( deltax * percent );
						//float texy		= Poly->vertices[i].tv + ( deltay * percent );
						copy = IntersectPoint;
						//copy.color		= Poly->vertices[i].color;
						//copy.tu			= texx; copy.tv = texy;


						BackList[BackCounter++]		= copy;
						FrontList[FrontCounter++]	= copy;
					}
				}

				//OK THEN LETS BUILD THESE TWO POLYGONAL BAD BOYS

				for(i = 0; i < FrontCounter; ++i)
				{
					verts.push_back(FrontList[i]);
				}

				if(newPoly)
				{

					for(i = 0; i < BackCounter; ++i)
					{
						newPoly->verts.push_back(BackList[i]);
					}

					newPoly->plane = plane;

					newPoly->clampToPlane(polyPlane);	clampToPlane(polyPlane);


				}

				return newPoly;

			}

			/**
			 * Returns the vector sum of all vertices's of this polygon.
			 */
			vertexType getVertexSum()
			{
				vertexType ret(0,0,0);

				const_iterator i;

				for(i = verts.begin(); i != verts.end(); ++i)
				{
					ret += (*i);
				}

				return ret;
			}

			/**
			 * Returns the center point of this polygon.
			 */
			vertexType getCenterPoint()
			{
				return getVertexSum() / (valueType)verts.size();
			}

			// @@@ TODO make a class to cache this stuff!
			/**
			 * Returns the normal of this polygon which is defined by the rotation 
			 * direction of its vertices's.
			 */
			vertexType getNormal()
			{
				vertexType ret(0,0,0);

				if(verts.size() > 2)
				{
					const_iterator vert = verts.begin();
					vertexType p1 = *vert++;
					vertexType p2 = *vert++;
					vertexType p3 = *vert;

					ret.cross(p2 - p1, p3 - p1);

					ret = ret.normalize();
				}

				return ret;

			}

			/**
			 * Returns this polygon's plane. The plane is calculated from the vertices's, 
			 * this polygons plane data member is not returned, or set by this function.
			 */
			Plane3 <valueType> * getPlane()
			{
				//return new Plane3 <valueType> (getCenterPoint(), getNormal()); // @@@ Centerpoint more accurate plane?

				if(verts.size()) { return new Plane3 <valueType> (*verts.begin(), -getNormal()) ;}
				
				else { return new Plane3 <valueType> () ;}
			}

			/**
			 * Clamps this polygons vertices's to the plane clampPlane.
			 */
			void clampToPlane ( const Plane3 <valueType> & clampPlane )
			{

				iterator i;

				for(i = verts.begin(); i != verts.end(); ++i)
				{
					valueType result = (*i).dot(clampPlane.normal) - clampPlane.dist;
					(*i) += (clampPlane.normal * -result);
				}
			}

			/**
			 * Constructs a very big polygon that is on the plane 
			 * @@@ From gtkRadiant
			 */
			void makeFromPlane(const Plane3 <valueType> & plane)
			{
				int		i, x;
				valueType	max, v;
				vertexType	org, vright;

				vertexType   vup; vup[0] = vup[1] = vup[2] = 0;

				// find the major axis

				max = -BOGUS_RANGE;
				x = -1;
				for (i=0 ; i<3; i++)
				{
					v = fabs(plane.normal[i]);
					if (v > max)
					{
						x = i;
						max = v;
					}
				}
				//if (x==-1)
				//Error ("Winding_BaseForPlane: no axis found");


				switch (x)
				{
				case 0:
				case 1:
					vup[2] = 1;
					break;
				case 2:
					vup[0] = 1;
					break;
				}

				v = vup.dot(plane.normal);

				vup += (plane.normal * -v);

				Vec3<valueType>::Normalize(vup);

				org = (plane.normal * plane.dist);

				Vec3<valueType>::CrossProduct (vup, plane.normal, vright);

				vup *= BOGUS_RANGE;

				vright *= BOGUS_RANGE;

				// project a really big	axis aligned box onto the plane

				verts.push_back((org - vright) + vup);

				verts.push_back((org + vright) + vup);

				verts.push_back((org + vright) - vup);

				verts.push_back((org - vright) - vup);

				clampToPlane(plane);
			}

			/**
			 * Creates a polygon that lines on the parameter plane, but is contained in 
			 * the axis aligned bounding box of parameter BB. Assumes that this polygon 
			 * contains no vertices's.
			*/
			void makeFromPlane(const Plane3 <valueType> & plane, const AABB3D < vertexType > & nodeBB)
			{
				const vertexType & planeNormal = plane.normal;

				vertexType BBCenter;
				vertexType V1;

				BBCenter = (nodeBB.max + nodeBB.min) / 2.0;

				float distFromPlane = BBCenter.dot(planeNormal) - plane.dist;

				V1 = BBCenter + (planeNormal * -distFromPlane);

				vertexType A; A[0] = A[1] = A[2] = 0;

				if(fabs(planeNormal.y) > fabs(planeNormal.z))
				{

					if(fabs(planeNormal.z) < fabs(planeNormal.x))
					{
						A.z = 1;

					}
					else
					{

						A.x = 1;

					}

				}
				else
				{

					if(fabs(planeNormal.y) <= fabs(planeNormal.x))
					{
						A.y = 1;

					}
					else
					{

						A.x = 1;

					}

				}

				vertexType U;
				vertexType V;

				U.cross(A, planeNormal);

				U.normalize();

				V.cross(U, planeNormal);

				V.normalize();

				float length = Vec3<valueType>::Length((nodeBB.max - BBCenter));

				U *= length;
				V *= length;

				verts.push_back(V1 + U - V); // Bottom right
				verts.push_back(V1 + U + V); // Top Right
				verts.push_back(V1 - U + V); // Top Left
				verts.push_back(V1 - U - V); // Bottom left

				clampToPlane(plane);

			}

			/**
			 * Calculates this polygon's axis aligned bounding box and returns it.
			 */
			AABB3D<valueType> findBoundingBox()
			{


				AABB3D<valueType> ret;

				const_iterator i;

				for(i = verts.begin(); i != verts.end(); ++i)
				{
					const vertexType & vert = *i;

					if(vert[0] < ret.min[0]) { ret.min[0] = vert[0] ;}
					if(vert[0] > ret.max[0]) { ret.max[0] = vert[0] ;}

					if(vert[1] < ret.min[1]) { ret.min[1] = vert[1] ;}
					if(vert[1] > ret.max[1]) { ret.max[1] = vert[1] ;}

					if(vert[2] < ret.min[2]) { ret.min[2] = vert[2] ;}
					if(vert[2] > ret.max[2]) { ret.max[2] = vert[2] ;}
				}
				return ret;
			}

			/**
			 * Finds bounding planes for this polygon, that is a bounding plane is created 
			 * for each edge of the polygon such that the edge's line segment rests on the 
			 * plane and the center of the bounding plane is in front of the bounding 
			 * plane. 
			 * @@@ Not tested!
			 */
			template <typename boundingPlanesContainer, typename boundingPlaneType>
			void getBoundingPlanes(boundingPlanesContainer & boundingPlanes)
			{
				const_iterator A;
				const_iterator B;

				vertexType normal = getNormal();

				vertexType edgeVector;

				vertexType boundingPlaneNormal;

				for(A = verts.begin(); A != verts.end(); ++A)
				{

					B = A;
					++B;
					if(B == verts.end()) { B = verts.begin() ;}

					edgeVector = (*B) - (*A);

					edgeVector.normalize();

					vertexType boundingPlaneNormal= vertexType::CrossProduct(normal,edgeVector);
					//boundingPlaneNormal = normal.cross(edgeVector);

					boundingPlanes.push_back(boundingPlaneType((*A), boundingPlaneNormal));

				}
			}
		};

		/**
		 * Templated vertex list container that can defined by the parameter vertexType. 
		 * Provides a definition to a constant and normal iterator of std::list 
		 * <vertexType>. Also provides a definition of the vertexType being vertType.
		 */
		template <typename vertexType>
		class vertexList
		{
			public:
				/**
				* List iterator
				*/
				typedef typename std::list< vertexType >::iterator iterator;
	
				/**
				* Constant list iterator
				*/
				typedef typename std::list< vertexType >::const_iterator const_iterator;

				typedef typename std::list< vertexType >::reverse_iterator reverse_iterator;	

				typedef typename std::list< vertexType >::const_reverse_iterator const_reverse_iterator;

				typedef typename std::list< vertexType >::size_type size_type;

				typedef typename std::list< vertexType >::value_type value_type;

				typedef vertexType vertType;

				vertexList() : verts()
				{
				}
				
				vertexList(const vertexList & vertexList)
				{
					// TODO
				}

				virtual ~vertexList()
				{
				}

				/**
				* stl list member functions
				*/

				inline iterator begin() { return verts.begin() ;}

				inline const_iterator begin() const { return verts.begin() ;}

				inline iterator end() { return verts.end() ;}

				inline const_iterator end() const { return verts.end() ;}

				inline reverse_iterator rbegin() { return verts.rbegin() ;}

				inline const_reverse_iterator rbegin() const { return verts.rbegin() ;}

				inline reverse_iterator rend() { return verts.rend() ;}

				inline const_reverse_iterator rend() const { return verts.rend() ;}

				inline bool empty() const { return verts.empty() ;}

				inline size_type size() const { return verts.size() ;}

				inline value_type & operator[] (const unsigned int i) const { return verts[i] ;}

				inline void push_back(const value_type vert) { verts.push_back(vert) ;}

				inline void clear() { verts.clear() ;}

			private:

				std::list<vertexType> verts;
				
		};

		/**
		 * Templated vertex vector container that can defined by the parameter vertexType. 
		 * Provides a definition to a constant and normal iterator of std::vector 
		 * <vertexType>. Also provides a definition of the vertexType being vertType.
		 */
		template <typename vertexType>
		class vertexVector
		{
			public:

				vertexVector() : verts() { ;}

				virtual ~vertexVector() { ;}
				/**
				* Vector iterator
				*/
				typedef typename std::vector< vertexType >::iterator iterator;
	
				/**
				* Constant vector iterator
				*/
				typedef typename std::vector< vertexType >::const_iterator const_iterator;

				typedef typename std::list< vertexType >::size_type size_type;

				typedef typename std::list< vertexType >::value_type value_type;

				/**
				* The type of vertices's stored in this container 
				*/
				typedef vertexType vertType;

				inline iterator begin() { return verts.begin() ;}

				inline const_iterator begin() const { return verts.begin() ;}

				inline iterator end() { return verts.end() ;}

				inline const_iterator end() const { return verts.end() ;}

				inline bool empty() const { return verts.empty() ;}

				inline size_type size() const { return verts.size() ;}

				inline value_type & operator[] (const unsigned int i) const { return verts[i] ;}

				inline void push_back(const value_type vert) { verts.push_back(vert) ;}

				inline void clear() { verts.clear() ;}

			private:

				std::vector < vertexType > verts;
		};

		typedef Polygon <vertexList < Vec3f >, Plane3f > Polygonlf;
		typedef Polygon <vertexVector < Vec3f >, Plane3f > Polygonvf;

		typedef Polygon <vertexList < Vec3d >, Plane3d > Polygonld;
		typedef Polygon <vertexVector < Vec3d >, Plane3d > Polygonvd;

		/**
		 * Templated polygon container. Definable by the container type and polygon type
		 */
		template <typename Container, typename POLY_TYPE>
		class polyContainer
		{
			public:
		
				polyContainer() : container() { ;}
				/**
				* The containers iterator
				*/
				typedef typename Container::iterator iterator;
	
				/**
				* The containers constant iterator
				*/
				typedef typename Container::const_iterator const_iterator;

				typedef typename Container::reverse_iterator reverse_iterator;	

				typedef typename Container::const_reverse_iterator const_reverse_iterator;

				typedef typename Container::size_type size_type;

				typedef typename Container::value_type value_type;	

				/**
				* The type of polygons stored in this container
				*/
				typedef POLY_TYPE polyType;

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

				inline void push_back(const value_type poly) { container.push_back(poly) ;}

			private:
				
				Container container;
		};

		typedef polyContainer <std::list   <Polygonlf *>, Polygonlf> polylfList;
		typedef polyContainer <std::vector <Polygonlf *>, Polygonlf> polyvfList;
		typedef polyContainer <std::list   <Polygonvf *>, Polygonvf> polylfVector;
		typedef polyContainer <std::vector <Polygonvf *>, Polygonvf> polyvfVector;

		typedef polyContainer <std::list   <Polygonld *>, Polygonld> polyldList;
		typedef polyContainer <std::vector <Polygonld *>, Polygonld> polyvdList;
		typedef polyContainer <std::list   <Polygonvd *>, Polygonvd> polyldVector;
		typedef polyContainer <std::vector <Polygonvd *>, Polygonvd> polyvdVector;
	}
}

#endif // __T_POLYGON_H__
