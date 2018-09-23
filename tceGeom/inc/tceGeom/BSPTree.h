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

#ifndef __BSP_TREE_H_
#define __BSP_TREE_H_
#include <tceGeom/BSPNode.h>
#include <tceGeom/face.h>
#include <tceGeom/leaf.h>
#include <tceGeom/AABB3D.h>

typedef unsigned int uint;

namespace tce
{
	namespace geom
	{
		class BSPTree
		{

			public:
	
				BSPTree() : planes(), nodes(), leafs(), faces(), portals(), boundingBox() { ;}
	
				~BSPTree() {;};
				void addPolygons(polylfList & polygons);
				void addPolygons(BSPFacePolyvfVector & polygons);
				void buildBSPTree();
	
				void clipTree(BSPTree & tree, bool clipSolid, bool removeCoPlaner, size_t nodeIndex = 0, BSPFacePolyvf * faces = NULL);
				void repairSplits();
	
				void getBoundingBox(AABB3Df & boundingBox);
				void getBoundingBox();
	
				bool intersects(BSPTree & BSPTree);
				bool intersects(BSPFacePolyvf & face, int nodeIndex = 0);
	
				void annotateTree(const Vec3f & pos, int nodeIndex = 0);
	
				void drawLeafs();
				
				BSPLeaf * findLeaf(const Vec3f & pos);
				int findLeafIndex(const Vec3f & pos, int nodeIndex = 0);
	
				void drawLeaf(size_t index);
	
				template <typename polyType>
				Plane3f * addPlane(polyType & poly)
				{
	
					Plane3f * plane = poly.getPlane();
	
					poly.clampToPlane(*plane);
	
					poly.plane = (int)planes.size();
	
					planes.push_back(plane);
	
					return plane;
	
				}
	
				BSPPlane3fVector planes;
				BSPNodeVector nodes;
				BSPLeafVector leafs;
				BSPFacePolyvfVector faces;
				TwoWayPortalVector portals;
				AABB3Df boundingBox;
	
			private:
				void findPlanes();
				template <typename faceContainer>
				BSPFacePolyvf * selectBestSplitter (const faceContainer & faces);
				size_t countRemainingSplitters(const BSPFacePolyvfVector & faces);
				template <typename faceContainer>
				void buildBSPTree(uint nodeIndex, faceContainer & faces);
		};
	}
}
#endif // __BSP_TREE_H_
