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
 
#ifdef WORKING

#include <iostream>
#include <tceGeom/HSR.h>
#include <tceGeom/BSPTree.h>

using namespace std;
using namespace tce::geom;
//using namespace tce::geom::quake3;

HiddenSurfaceRemoval::HiddenSurfaceRemoval() : faces(), brushConverter(), meshes()
{
	
}

HiddenSurfaceRemoval::HiddenSurfaceRemoval(const HiddenSurfaceRemoval & hiddenSurfaceRemoval)  : faces(), brushConverter(), meshes()
{
	// @@@ TODO
}


HiddenSurfaceRemoval::~HiddenSurfaceRemoval()
{

}

HiddenSurfaceRemoval &
HiddenSurfaceRemoval::operator= (const HiddenSurfaceRemoval & hiddenSurfaceRemoval)
{
	// @@@ TODO

	return *this;
}

/*
void 
HiddenSurfaceRemoval::setQ3Map(Q3BSPMap * map)
{
	brushConverter.setQ3Map(map);
}
*/

void
HiddenSurfaceRemoval::setBSPTree(BSPTree * BSPTree)
{
	BSPPolyPlaneAccess < Plane3f, BSPFacePolyvfVector::polyType > planeAccess(BSPTree);
	
	brushConverter.setPlaneAccess(planeAccess);
}

void
HiddenSurfaceRemoval::addBrush(MapBrushf & mapBrush)
{
	addMesh(brushConverter.convert(mapBrush));
}

/*
void 
HiddenSurfaceRemoval::addBrush(Q3BSPBrush & brush)
{	
	addMesh(brushConverter.convert(brush));
}
*/

void
HiddenSurfaceRemoval::addMesh(BSPFacePolyvfVector * mesh)
{
	meshes.push_back(mesh);
}

void
HiddenSurfaceRemoval::doHSR()
{
	cout << "doHSR" << endl;
	
	unique_ptr<BSPTree[]> trees(new BSPTree[meshes.size()]);

	std::list <BSPFacePolyvfVector*>::const_iterator mesh;

	BSPTree * treesPtr = trees.get();

	for(mesh = meshes.begin(); mesh != meshes.end(); ++mesh)
	{
		treesPtr->addPolygons(**mesh);

		treesPtr->buildBSPTree();

		++treesPtr;
	}

	size_t i;

	size_t j;

	size_t numMeshes = meshes.size();
	
	// @@@ Might be able to combine this with another process to speed things up more.
	for(i = 0; i < numMeshes; ++i)
	{
		BSPTree & tree = trees[i];
		
		tree.getBoundingBox();
	}
	
	for(i = 0; i < numMeshes; ++i)
	{
		BSPTree & treeA = trees[i];
		
		cout << i << " - " << meshes.size();
		
		if(treeA.faces.empty()) { continue ;}
		
		AABB3Df & bboxA = treeA.boundingBox;
			
		for(j = i + 1; j < numMeshes; ++j)
		{
		
			BSPTree & treeB = trees[j];

			if(treeB.faces.empty()) { continue ;}

			const AABB3Df & bboxB = treeB.boundingBox;
			
			if(!bboxA.intersects(bboxB, 0.1)) { continue ;}

			if(!treeA.intersects(treeB)) { continue ;}

			cout << ".";
			
			treeA.clipTree( treeB, true, false );
			
			treeB.clipTree( treeA, true, true  );
			
			treeA.repairSplits();
			
			treeB.repairSplits();
		}
		
		cout << endl;
		
		getPolys(treeA);
	}
	
	//delete [] trees;
	
	cout << "doHSR done" << endl;
}

void
HiddenSurfaceRemoval::getPolys(BSPTree & BSPTree)
{
	BSPFacePolyvfVector::const_iterator i;
	
	for(i = BSPTree.faces.begin(); i != BSPTree.faces.end(); ++i)
	{
		BSPFacePolyvf * face = *i;
		
		if(face->deleted) { continue ;}
		
		if(face->verts.size() < 3)
		{
			// @@@ Can this happen?
			cout << "@@@ face->vertes.size() = " << face->verts.size() << endl;
			continue;
		}
		
		faces->push_back(face);
	}
}

#endif