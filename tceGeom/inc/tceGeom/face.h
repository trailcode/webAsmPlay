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

#ifndef __FACE_H_
#define __FACE_H_

#include <vector>
#include <list>
#include <tceGeom/polygon.h>

namespace tce
{
	namespace geom
	{

		template <typename polyType>
		struct BSPFace : Polygon <typename polyType::vertexContainerType, int>
		{

			bool      usedAsSplitter;         // Polygon used as splitter
			bool      deleted;                // Face has been 'Virtually' deleted
			int       childSplit[2];          // During CSG, records the resulting 2 children
			BSPFace  *next;                   // next poly in linked list

			float color[3];

			bool groundFace;

			typedef typename polyType::vertexContainerType::const_iterator const_iterator;

			typedef typename polyType::vertexType vertexType;

			typedef typename polyType::valueType valueType;

			BSPFace() : Polygon <typename polyType::vertexContainerType, int>(), usedAsSplitter(false), deleted(false), next(NULL), groundFace(false)
			{
				init();
			}

			BSPFace(const BSPFace & face) : Polygon <typename polyType::vertexContainerType, int>(), usedAsSplitter(false), deleted(false), next(NULL), groundFace(false)
			{
				// TODO
				init();
			}

			BSPFace ( typename polyType::vertexContainerType & _verts ) : Polygon <typename polyType::vertexContainerType, int> (_verts), usedAsSplitter(false), deleted(false), next(NULL), groundFace(false)
			{

				init();

			};

			BSPFace (const Plane3 < valueType > & plane, const AABB3D < vertexType > & BB)
					: Polygon <typename polyType::vertexContainerType, int> (plane, BB), usedAsSplitter(false), deleted(false), next(NULL), groundFace(false)
			{

				init();

			}

			BSPFace (const Plane3 < valueType > & plane) : Polygon <typename polyType::vertexContainerType, int>(), usedAsSplitter(false), deleted(false), next(NULL), groundFace(false)
			{
				init();

				Polygon <typename polyType::vertexContainerType, int>::makeFromPlane(plane);

			}

			virtual ~BSPFace()
			{
			}

			BSPFace & operator=(const BSPFace & face)
			{
				// TODO
				return *this;
			}

			void init()
			{
				color[0] = ((float)(rand() % 256))/256.0;
				color[1] = ((float)(rand() % 256))/256.0;
				color[2] = ((float)(rand() % 256))/256.0;

				childSplit[0] = -1;
				childSplit[1] = -1;
				this->plane = -1;
			}

			AABB3Df findBoundingBox()
			{ // @@@ Move to Polygon


				AABB3Df ret;

				const_iterator i;

				for(i = this->verts.begin(); i != this->verts.end(); ++i)
				{
					ret.grow(*i);
				}
				
				return ret;
			}

			BSPFace * splitFace(Plane3 < valueType > & spliPlane, const Plane3 < valueType > facePlane)
			{
				BSPFace * newFace = new BSPFace();

				if(Polygon <typename polyType::vertexContainerType, int>::splitPolygon(spliPlane, facePlane, (Polygon <typename polyType::vertexContainerType, int> *)newFace)) { return newFace ;}

				delete newFace;
					
				return NULL;
			}
		};

		typedef BSPFace < Polygon <vertexList   < Vec3f >, int > > BSPFacePolylf;
		typedef BSPFace < Polygon <vertexVector < Vec3f >, int > > BSPFacePolyvf;

		/**
		 * Template face container wrapper, provides a constant and normal iterator 
		 * to the container.
		 */
		template <typename Container, typename _polyType>
		class BSPFaceContainer
		{
			public:
				BSPFaceContainer() : container() { ;}

				virtual ~BSPFaceContainer() { ;}

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
				* The type of polygons stored in this container
				*/
				typedef _polyType polyType;

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

				inline void push_back(const value_type face) { container.push_back(face) ;}

			private:

				Container container;
		};

		typedef BSPFaceContainer <std::vector < BSPFacePolylf * >, BSPFacePolylf> BSPFacePolylfVector;
		typedef BSPFaceContainer <std::list   < BSPFacePolylf * >, BSPFacePolylf> BSPFacePolylfList;
		typedef BSPFaceContainer <std::vector < BSPFacePolyvf * >, BSPFacePolyvf> BSPFacePolyvfVector;
		typedef BSPFaceContainer <std::list   < BSPFacePolyvf * >, BSPFacePolyvf> BSPFacePolyvfList;

	}
}

#endif
