#ifdef WORKING

#include <iostream>
//#include <SDL.h>
//#include <GL/glew.h>
#ifdef APPLE
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif
//#include <glew/glew.h>
#include <tceGeom/polygon.h>
#include <tceGeom/BSPTree.h>
#include <tceGeom/brushConverter.h>
//#include "connectingSegment.h"

using namespace  std;
using namespace tce::geom;

void
BSPTree::addPolygons(polylfList & polygons)
{
	/*
	polylfList::const_iterator i;

	BSPFacePolyvf *prevFace = NULL;

	for(i = polygons.begin(); i != polygons.end(); ++i)
	{
		BSPFacePolyvf *face = new BSPFacePolyvf((**i).verts);
		
		face.usedAsSplitter = false;
		
		face.deleted = false;
		
		faces.push_back(face);
		
		if(prevFace) { prevFace->next = face ;}
		
		prevFace = face;
		
	}
	*/
}

void
BSPTree::addPolygons(BSPFacePolyvfVector & polygons)
{
	BSPFacePolyvfVector::const_iterator i;

	BSPFacePolyvf *prevFace = NULL;

	for(i = polygons.begin(); i != polygons.end(); ++i)
	{

		(*i)->usedAsSplitter = (*i)->deleted = false;

		faces.push_back(*i);

		if(prevFace) { prevFace->next = *i ;}

		prevFace = *i;
	}
}

// @@@ This is slow, consider a hashing to speed it up
void
BSPTree::findPlanes()
{

	cout << "BSPTree::findPlanes()" << endl;

	BSPFacePolyvfVector::const_iterator i;

	for(i = faces.begin(); i != faces.end(); ++i)
	{

		BSPFacePolyvf & face = **i;

		Plane3f * plane = face.getPlane();

		face.clampToPlane(*plane);

		int planeIndex = planes.getPlaneIndex(*plane);
		
		if(planeIndex == -1)
		{
			face.plane = (int)planes.size();

			planes.push_back(plane);
		}
		else { face.plane = planeIndex ;}
	}
	
	cout << "BSPTree::findPlanes() done" << endl;
}


template <typename faceContainer>
BSPFacePolyvf *
BSPTree::selectBestSplitter (const faceContainer & faces)
{
	if(faces.size() == 1) { return *faces.begin() ;}

	int bestScore = 1000000;	// just set to a very high value to begin
	
	BSPFacePolyvf *selectedPoly = NULL;

	int numTries = 1000;

	typename faceContainer::const_iterator splitter;
	typename faceContainer::const_iterator poly;

	for(splitter = faces.begin(); splitter != faces.end(); ++splitter)
	{
		if((*splitter)->usedAsSplitter) { continue ;}

		int frontFaces = 0;
		int backFaces = 0;
		int splits = 0;

		int score;

		for(poly = faces.begin(); poly != faces.end(); ++poly)
		{
			if(splitter == poly) { continue ;}

			Plane3f & plane = *planes[(*splitter)->plane];

			int result = (*poly)->classifyPlane(plane);

			switch ( result )
			{
			case SIDE_ON:
				break;
			case SIDE_FRONT:
				frontFaces++;
				break;
			case SIDE_BACK:
				backFaces++;
				break;
			case SIDE_BOTH:
				splits++;
				break;
			default:
				break;
			}
		}

		score = abs(frontFaces-backFaces) + (splits*8);

		if(score < bestScore)
		{
			bestScore = score;
			
			selectedPoly = *splitter;
		}

		if(!(--numTries)) { break ;}
	}

	return selectedPoly;
}

size_t
BSPTree::countRemainingSplitters(const BSPFacePolyvfVector & faces)
{
	int ret = 0;

	BSPFacePolyvfVector::const_iterator face;

	for(face = faces.begin(); face != faces.end(); ++face)
	{
		if(!(*face)->usedAsSplitter) { ++ret ;}
	}

	return ret;
}

void
BSPTree::buildBSPTree()
{

	//cout << "buildBSPTree" << endl;

	findPlanes();

	nodes.push_back(new BSPNode);

	if(faces.empty()) { return ;}

	buildBSPTree<BSPFacePolyvfVector>(0, faces);

	//findClusters();

	//cout << "buildBSPTree done" << endl;

}

template <typename faceContainer>
void
BSPTree::buildBSPTree(uint nodeIndex, faceContainer & faces)
{

	BSPFacePolyvfVector frontFaces;
	BSPFacePolyvfVector backFaces;

	BSPFacePolyvf * splitter = selectBestSplitter<faceContainer>(faces);

	if(!splitter) { return ;}
	
	splitter->usedAsSplitter = true;

	BSPNode * node = nodes[nodeIndex];

	node->planeIndex = splitter->plane;

	Plane3f & splitterPlane = *planes[splitter->plane];

	int result;

	typename faceContainer::const_iterator i;

	for(i = faces.begin(); i != faces.end(); ++i)
	{
		BSPFacePolyvf * currFace = *i;

		const Plane3f & currFacePlane = *planes[currFace->plane];

		// @@@ Also same facing, optimize in switch below
		if(currFace->plane == splitter->plane) { result = SIDE_ON ;}
		
		else { result = currFace->classifyPlane ( splitterPlane ) ;}

		switch ( result )
		{
		case SIDE_FRONT:

			frontFaces.push_back(currFace);

			break;

		case SIDE_BACK:

			backFaces.push_back(currFace);

			break;

		case SIDE_ON:

			currFace->usedAsSplitter = true; // @@@ Look into this : Mark matching planes as used (Accuracy may be decreased)

			if(splitterPlane.sameFacing(currFacePlane)) { frontFaces.push_back(currFace) ;}
			
			else { backFaces.push_back(currFace) ;}

			break;

		case SIDE_BOTH:

			if(currFace == splitter) { cerr << "Invalid geometry" << endl ;}

			BSPFacePolyvf * newFace = currFace->splitFace(splitterPlane, currFacePlane);

			newFace->plane = currFace->plane;

			frontFaces.push_back(currFace);
			backFaces.push_back(newFace);

			// Allow split fragments to generate their own node (Accuracy may be increased)
			currFace->usedAsSplitter = false;
			newFace->usedAsSplitter = false;

			break;

		}
	}

	BSPFacePolyvfVector::const_iterator face;

	for(face = frontFaces.begin(); face != frontFaces.end(); ++face)
	{

		AABB3Df & nodeBB = node->boundingBox;

		Polygonvf::const_iterator vert;
		for(vert = (*face)->verts.begin(); vert != (*face)->verts.end(); ++vert)
		{
			nodeBB.grow(*vert);
		}
	}

	for(face = backFaces.begin(); face != backFaces.end(); ++face)
	{

		AABB3Df & nodeBB = node->boundingBox;

		Polygonvf::const_iterator vert;
		for(vert = (*face)->verts.begin(); vert != (*face)->verts.end(); ++vert)
		{
			nodeBB.grow(*vert);
		}
	}

	//annotateTree(frontFaces, backFaces, splitter);

	if(!countRemainingSplitters(frontFaces))
	{
		leafs.push_back ( new BSPLeaf ( frontFaces , nodeIndex) );

		nodes[nodeIndex]->frontIndex = -leafs.size();
	}
	else if(!frontFaces.empty())
	{
		const int newNodeIndex = nodes.size();

		nodes[nodeIndex]->frontIndex = newNodeIndex;

		nodes.push_back(new BSPNode ( nodeIndex ) );

		buildBSPTree<BSPFacePolyvfVector>(newNodeIndex, frontFaces);
	}

	if(true) // Back leaf cull
	{
		//if(!countRemainingSplitters(backFaces))
		{
			/*
			while(backFaces)
			{
				BSPFacePolyvf * currFace = backFaces;
				backFaces = backFaces->next;
				delete currFace;
			}
			*/

			//nodes[nodeIndex]->backIndex = -1;

			//delete backFaces;

		}
		//else
		
		if(!backFaces.empty())
		{
			const int newNodeIndex = nodes.size();

			nodes[nodeIndex]->backIndex = newNodeIndex;

			nodes.push_back(new BSPNode(nodeIndex));

			buildBSPTree<BSPFacePolyvfVector>(newNodeIndex, backFaces);
			
		} else {
			
			nodes[nodeIndex]->backIndex = -1;
			
		}
	}
}

void
BSPTree::clipTree(BSPTree & tree, bool clipSolid, bool removeCoPlaner, size_t nodeIndex, BSPFacePolyvf * faces)
{
	BSPFacePolyvf * face;
	BSPFacePolyvf * nextFace;
	BSPFacePolyvf * frontList  = NULL;
	BSPFacePolyvf * backList   = NULL;
	BSPFacePolyvf * frontSplit = NULL;
	BSPFacePolyvf * backSplit  = NULL;

	if(!faces)
	{
		BSPFacePolyvfVector::iterator i = tree.faces.begin();

		face = *i;
		
		faces = face;
		
		BSPFacePolyvf * prevFace = face;

		face->childSplit[0] = -1;
		face->childSplit[1] = -1;

		for(++i; i != tree.faces.end(); ++i)
		{
			face = *i;

			face->childSplit[0] = -1;
			face->childSplit[1] = -1;

			prevFace->next = face;
			
			prevFace = face;
		}

		prevFace->next = NULL;
	}

	const BSPNode & currNode = *(nodes[nodeIndex]);

	Plane3f & plane = *planes[currNode.planeIndex];

	BSPFacePolyvfList::iterator i;

	for(face = faces; face != NULL; face = nextFace)
	{
		nextFace = face->next;

		if(face->deleted) { continue ;}

		size_t result = face->classifyPlane ( plane );

		switch ( result )
		{
		case SIDE_ON:

			if(plane.sameFacing(*(tree.planes[face->plane])))
			{
				if(removeCoPlaner)
				{
					face->next = backList;
					backList = face;
				}
				else
				{
					face->next = frontList;
					frontList = face;
				}
			}
			else
			{
				face->next = backList;
				backList = face;
			}

			break;

		case SIDE_FRONT:

			face->next = frontList;
			frontList = face;

			break;

		case SIDE_BACK:

			face->next = backList;
			backList = face;

			break;

		case SIDE_BOTH:

			frontSplit = new BSPFacePolyvf;
			backSplit = new BSPFacePolyvf;

			face->splitPolygon(plane, *tree.planes[face->plane], frontSplit,backSplit);

			tree.faces.push_back(frontSplit);
			tree.faces.push_back(backSplit);

			face->deleted = true;
			face->childSplit[0] = tree.faces.size() - 2;
			face->childSplit[1] = tree.faces.size() - 1;

			frontSplit->next = frontList;
			frontList = frontSplit;
			backSplit->next = backList;
			backList = backSplit;

			break;
		}
	}

	if(clipSolid)
	{
		if(currNode.backIndex < 0)
		{
			for(face = backList; face != NULL; face = face->next) { face->deleted = true ;}

			backList = NULL;
		}
	}
	else
	{
		if(currNode.frontIndex < 0)
		{
			for(face = frontList; face != NULL; face = face->next) { face->deleted = true ;}

			frontList = NULL;
		}
	}

	if(frontList && currNode.frontIndex >= 0) { clipTree(tree, clipSolid, removeCoPlaner, currNode.frontIndex, frontList) ;}

	if(backList && currNode.backIndex >= 0) { clipTree(tree, clipSolid, removeCoPlaner, currNode.backIndex, backList) ;}
}

void
BSPTree::getBoundingBox(AABB3Df & boundingBox)
{
	boundingBox.reset();

	BSPFacePolyvfVector::const_iterator i;

	for(i = faces.begin(); i != faces.end(); ++i)
	{
		Polygonvf::const_iterator vert;

		BSPFacePolyvf * face = *i;

		for(vert = face->verts.begin(); vert != face->verts.end(); ++vert)
		{
			boundingBox.grow(*vert);
		}
	}
}

void
BSPTree::getBoundingBox()
{
	getBoundingBox(boundingBox);
}

bool
BSPTree::intersects(BSPTree & BSPTree)
{
	BSPFacePolyvfVector::const_iterator face;

	for(face = faces.begin(); face != faces.end(); ++face)
	{
		if(intersects(**face)) { return true ;}
	}

	return false;
}

bool
BSPTree::intersects(BSPFacePolyvf & face, int nodeIndex)
{
	if(nodeIndex < 0) { return false ;}

	const BSPNode & node = *nodes[nodeIndex];

	int frontNodeIndex = node.frontIndex;
	int  backNodeIndex = node.backIndex;

	Plane3f & plane = *planes[node.planeIndex];

	switch(face.classifyPlane(plane))
	{
	case SIDE_BOTH:
	case SIDE_ON:

		// If it is in a solid leaf
		if(backNodeIndex < 0) { return true ;}

		// Go down the front
		if(frontNodeIndex >= 0 && intersects(face, frontNodeIndex)) { return true ;}

		// Go down the back
		if(backNodeIndex >= 0 && intersects(face, backNodeIndex)) { return true ;}

		break;

	case SIDE_FRONT:

		// Go down the front
		if(frontNodeIndex >= 0 && intersects(face, frontNodeIndex)) { return true ;}

		break;

	case SIDE_BACK:

		// If it is in a solid leaf
		if(backNodeIndex < 0) { return true ;}

		// Go down the back
		if(backNodeIndex >= 0 && intersects(face, backNodeIndex)) { return true ;}

		break;

	}

	// There was no intersection at this node
	return false;
}

void
BSPTree::repairSplits()
{
	//BSPFacePolyvfVector::iterator i;
	BSPFacePolyvf * face;
	BSPFacePolyvf * childSplitA;
	BSPFacePolyvf * childSplitB;

	for(int i = faces.size() - 1; i >= 0; --i)
	{
		face = faces[i];

		if(face->childSplit[0] == -1) { continue ;}

		if(face->childSplit[0] >= (int)faces.size()) { cout << "@@@ Error" << endl; continue ;}
		
		if(face->childSplit[1] >= (int)faces.size()) { cout << "@@@ Error" << endl; continue ;}

		childSplitA = faces[face->childSplit[0]];
		
		childSplitB = faces[face->childSplit[1]];

		if(childSplitA->deleted && childSplitB->deleted)
		{
			face->deleted = false;

			childSplitB->deleted = childSplitA->deleted = true;
		}
	}
}

void
BSPTree::annotateTree(const Vec3f & pos, int nodeIndex)
{
	if(nodeIndex < 0) { drawLeaf(~nodeIndex); return ;}

	const BSPNode & node = *nodes[nodeIndex];

	int result = planes[node.planeIndex]->classifyPoint(pos);

	if (result==SIDE_FRONT)
	{
		if (node.backIndex != -1) { annotateTree (pos, node.backIndex) ;}

		if (node.frontIndex < 0 && node.frontIndex != -1) { drawLeaf(~node.frontIndex) ;}
		
		else { annotateTree (pos, node.frontIndex) ;}

		return;
	}
	//if this happens if we are at back or on plane

	if(node.frontIndex >= 0) { annotateTree (pos, node.frontIndex) ;}
	
	else if (node.frontIndex != -1) { drawLeaf(~node.frontIndex) ;}

	if(node.backIndex != -1) { annotateTree (pos, node.backIndex) ;}

	return;
}

void
BSPTree::drawLeafs() {
	
	for(int i = 0; i < leafs.size(); ++i) { drawLeaf(i) ;}
	
}

BSPLeaf *
BSPTree::findLeaf(const Vec3f & pos) { return leafs[findLeafIndex(pos)] ;}

int
BSPTree::findLeafIndex(const Vec3f & pos, int nodeIndex)
{
	if(nodeIndex < 0) { return ~nodeIndex ;}

	const BSPNode & node = *nodes[nodeIndex];

	int result = planes[node.planeIndex]->classifyPoint(pos);

	if (result==SIDE_FRONT) { return findLeafIndex(pos, node.frontIndex) ;}

	return findLeafIndex(pos, node.backIndex);
}

void
BSPTree::drawLeaf(size_t leafIndex)
{
	BSPLeaf * hi = leafs[leafIndex];
	const BSPFacePolyvfVector & hello = hi->faces;

	BSPFacePolyvfVector::const_iterator i;

	for(i = hello.begin(); i != hello.end(); ++i)
	{
		//glBegin(GL_LINE_STRIP);
		glBegin(GL_POLYGON);

		//glColor3fv((*i)->color);
		glColor4f((*i)->color[0], (*i)->color[1], (*i)->color[2], 0.5);

		vertexVector < Vec3f >::const_iterator j;
		for(j = (*i)->verts.begin(); j != (*i)->verts.end(); ++j) { glVertex3f((*j).x,(*j).y,(*j).z) ;}
		
		glEnd();
		
		glBegin(GL_LINES);
			
			glColor3f(1,1,1);
			
			Vec3f center = (*i)->getCenterPoint();
			
			center.glVertex();
			
			(center + ((*i)->getNormal() * 25.0f)).glVertex();
			
		glEnd();
	}
}



#endif