#include <iostream>
#include <tceGeom/polygon.h>
#include <tceGeom/BSPTree.h>
//#include <tceGeom/quake3/Q3BSPMap.h>
#include <tceGeom/brushConverter.h>
#include <tceGeom/AABB3D.h>
#include <tceGeom/face.h>

using namespace std;
using namespace tce::geom;
//using namespace tce::geom::quake3;

/*
template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter() : map(NULL), planeAccess() { ;}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter(const polyPlaneAccess & _planeAccess) : map(NULL), planeAccess(_planeAccess) { ;}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter(const BrushConverter & converter) : map(NULL), planeAccess()
{
	// @@@ TODO
}
*/

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter() : planeAccess() { ;}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter(const polyPlaneAccess & _planeAccess) : planeAccess(_planeAccess) { ;}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::BrushConverter(const BrushConverter & converter) : planeAccess()
{
	// @@@ TODO
}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess>::~BrushConverter()
{
	
}

template <typename polyContainer, typename polyPlaneAccess>
BrushConverter<polyContainer, polyPlaneAccess> & BrushConverter<polyContainer, polyPlaneAccess>::operator= (const BrushConverter & converter)
{
	// @@@ TODO

	return *this;
}

/*
template <typename polyContainer, typename polyPlaneAccess>
void
BrushConverter<polyContainer, polyPlaneAccess>::setQ3Map(Q3BSPMap * map)
{
	this->map = map;
}
*/

template <typename polyContainer, typename polyPlaneAccess>
void
BrushConverter<polyContainer, polyPlaneAccess>::setPlaneAccess(const polyPlaneAccess & planeAccess)
{
	this->planeAccess = planeAccess;
}

/*
template <typename polyContainer, typename polyPlaneAccess>
polyContainer * 
BrushConverter<polyContainer, polyPlaneAccess>::convert(Q3BSPBrush &brush, polyContainer & polygons)
{
	
	// List to hold planes created;
	planefList planes;
	
	// For each brush side.
	for(size_t i = 0; i < (size_t)brush.numBrushSides; ++i)
	{
		
		Q3BSPBrushSide & brushSide = P_BRUSH_SIDES[brush.brushSideIndex + i];
		
		Plane3Df plane(P_PLANES[brushSide.planeIndex]);
		
		plane.invert();
		
		planes.push_back(plane);
	
	}
	
	//expandBrush(planes, Vec3f(14,32,14));
	
	//addBevels(planes, tmpPolys);
	
	convert(planes, polygons);
	
	return & polygons;
}
*/

/**
 * Converts a brush to a list of polygons. 
 */
template <typename polyContainer, typename polyPlaneAccess>
polyContainer * 
BrushConverter<polyContainer, polyPlaneAccess>::convert(MapBrushf &brush, polyContainer & polygons)
{
	
	// List to hold planes created;
	planefList planes;
	
	MapBrushf::iterator i;
	
	// For each brush side.
	for(i = brush.brushSides.begin(); i != brush.brushSides.end(); ++i)
	{
		MapBrushSidef &brushSide = (*i);
		
		Plane3f plane (brushSide.plane.points[0],brushSide.plane.points[1],brushSide.plane.points[2]);
		
		planes.push_back(plane);
	}
	
	//expandBrush(planes, Vec3f(5,5,5));
	
	//addBevels(planes, tmpPolys);
	
	convert(planes, polygons);
	
	return & polygons;
}

/*
template <typename polyContainer, typename polyPlaneAccess>
polyContainer *
BrushConverter<polyContainer, polyPlaneAccess>::convert(Q3BSPBrush &brush)
{
	return convert(brush, *(new polyContainer));
}
*/

template <typename polyContainer, typename polyPlaneAccess>
polyContainer * 
BrushConverter<polyContainer, polyPlaneAccess>::convert(MapBrushf &brush)
{
	return convert(brush, *(new polyContainer));
}

template <typename polyContainer, typename polyPlaneAccess>
void 
BrushConverter<polyContainer, polyPlaneAccess>::addBevels(planefList & planes, polyContainer & polys)
{
	polyContainer_iterator i;
	
	AABB3Df box;
	
	Vec3f axisis[6] = {Vec3f(1,0,0),Vec3f(0,1,0),Vec3f(0,0,1),
							 Vec3f(-1,0,0),Vec3f(0,-1,0),Vec3f(0,0,-1)};
							 
	bool bevelAxis[6] = {true,true,true,true,true,true};
	
	for(i = polys.begin(); i != polys.end(); ++i)
	{
		polyType & poly = **i;
		
		const Vec3f & planeNormal = planeAccess.getPlane(poly)->normal;
		
		for(size_t j = 0; j < 6; ++j) { bevelAxis[j] &= !(axisis[j].fuzzyCompare(planeNormal,1e-5f)) ;}
		
		polyVertexConstIterator vert;
		
		for(vert = poly.verts.begin(); vert != poly.verts.end(); ++vert) { box.addInternalPoint(*vert) ;}
	}

	box.expand(Vec3f(5,5,5));
	
	// Had to do this. Cound not use the methods in tBoundingBox
	// After making this class a template strange things happened with 
	// the getBoundingPlanes function. For one it would not compile. 
	// I made a C style function and passed the data to it. It compiled,
	// but for the ".map" files no bevel planes were added. But for the
	// brushes obtained from a bsp file, the bevels were added fine.
	// I don't get it. Well it seems to be working now. 
	const Vec3f extent = box.getExtent();
	const Vec3f cp = box.getCentroid();
	
	if(bevelAxis[0]) { planes.push_back(box.get1stPlane(cp, extent)) ;}
	if(bevelAxis[1]) { planes.push_back(box.get2ndPlane(cp, extent)) ;}
	if(bevelAxis[2]) { planes.push_back(box.get3ndPlane(cp, extent)) ;}
	if(bevelAxis[3]) { planes.push_back(box.get4thPlane(cp, extent)) ;}
	if(bevelAxis[4]) { planes.push_back(box.get5thPlane(cp, extent)) ;}
	if(bevelAxis[5]) { planes.push_back(box.get6thPlane(cp, extent)) ;}

	
}

template <typename polyContainer, typename polyPlaneAccess>
bool
BrushConverter<polyContainer, polyPlaneAccess>::tryAddBevelPlane(planeType & plane, planefList & planes, const polyContainer & polys)
{
	assert(false); // Fix me! Does not compile for double

	/*
	polyContainer_const_iterator i;
	
	for(i = polys.begin(); i != polys.end(); ++i)
	{
		const polyType & poly = **i;
					
		if(poly.classifyPlane(plane) != SIDE_BACK) { return false ;}
	}
	
	planefList::const_iterator j;
	
	for(j = planes.begin(); j != planes.end(); ++j)
	{
		if(plane.samePlane(*j)) { return false;}
	}
	
	planes.push_back(plane);
	*/

	return true;
}

template <typename polyContainer, typename polyPlaneAccess>
void 
BrushConverter<polyContainer, polyPlaneAccess>::expandBrush(planefList &brushSides, const Vec3f & AABB)
{
		
	planefList::iterator i;
	
	for(i = brushSides.begin(); i != brushSides.end(); ++i)
	{	
	
		Plane3f & plane = *i;
		
		float offset = fabs (AABB.x * plane.normal.x) +
							fabs (AABB.y * plane.normal.y) +
							fabs (AABB.z * plane.normal.z);
		
		plane.dist -= offset;
		
	}
}

template <typename polyContainer, typename polyPlaneAccess>
void
BrushConverter<polyContainer, polyPlaneAccess>::convert(planefList &brushSides, polyContainer & polygons)
{
	polyContainer tmpPolys;
	
	planefList::const_iterator i;
	
	for(i = brushSides.begin(); i != brushSides.end(); ++i) { tmpPolys.push_back(new polyType(*i)) ;}
	
	planefList::const_iterator plane;
	
	polyContainer_iterator poly;
	
	// For each plane ... 
	for(plane = brushSides.begin(); plane != brushSides.end(); ++plane)
	{
		// For each polygon clip it by all the planes.
		for(poly = tmpPolys.begin(); poly != tmpPolys.end(); ++poly) { windingClip(*plane, (**poly)) ;}
	}
	
	for(poly = tmpPolys.begin(); poly != tmpPolys.end(); ++poly)
	{
		if((**poly).verts.size() > 2) { polygons.push_back(*poly) ;}
	}	
}


/**
 * From Q3Radiant
 *
 * Clips a winding with a plane. The winding must be convex.
 *
 */
template <typename polyContainer, typename polyPlaneAccess>
void
BrushConverter<polyContainer, polyPlaneAccess>::windingClip(const Plane3f & plane, polyType & polygon)
{
	polyTypeVertexContainer & winding = polygon.verts;
	
	int numOfVerts = winding.size();
	
	unique_ptr<float[]> dists(new float[numOfVerts + 1]);
	unique_ptr<int[]>   sides(new int  [numOfVerts + 1]);
	int   counts[3];
	
	float dot;
	
	int i = 0;
	int j;
	
	polyVertexConstIterator vert;
	
	// Find out what has to be done to the winding.
	for(vert = winding.begin(); vert != winding.end(); ++vert)
	{
		// Find what side the current vert is in relation to the plane.
		dot = vert->dot(plane.normal) - plane.dist;
		
		dists[i] = dot;

		if (dot > ON_EPSILON) { sides[i] = SIDE_FRONT ;} 
		
		else if (dot < -ON_EPSILON) { sides[i] = SIDE_BACK ;} 
		
		else { sides[i] = SIDE_ON ;}
		
		counts[sides[i++]]++;		
	}
	
	sides[i] = sides[0];
	dists[i] = dists[0];
	
	// Winding is exactly on plane, no mods to winding.
	if(!counts[0] && !counts[1]) { return ;}
	
	// Compleatly remove winding
	if (!counts[0]) 
	{
		winding.clear();
		
		return;
	}
	
	// Don't have to change winding
	if (!counts[1]) { return ;}
				
	// Must clip winding
	unique_ptr<Vec3f[]> newWinding(new Vec3f[numOfVerts]);
				
	int count = 0;
	
	for(vert = winding.begin(); vert != winding.end(); ++vert) { newWinding[count++] = *vert ;}
	
	winding.clear();
	
	for(i = 0; i < numOfVerts; ++i)
	{
		const Vec3f & p1 = newWinding[i];
		
		if (sides[i] == SIDE_ON)
		{
			winding.push_back(p1);
			
			continue;
		}
	
		if (sides[i] == SIDE_FRONT) { winding.push_back(p1) ;}
		
		if (sides[i+1] == SIDE_ON || sides[i+1] == sides[i]) { continue ;}
		
		// generate a split point
		const Vec3f & p2 = newWinding[(i+1)%numOfVerts];
		
		dot = dists[i] / (dists[i]-dists[i+1]);
		
		Vec3f mid;
		
		for (j=0 ; j<3 ; j++)
		{	
			// avoid round off error when possible
			if (plane.normal[j] == 1) { mid[j] = plane.dist ;}
			
			else if (plane.normal[j] == -1) { mid[j] = -plane.dist ;}
			
			else { mid[j] = p1[j] + dot*(p2[j]-p1[j]) ;}
		}
		
		winding.push_back(mid);
		
	}
}

namespace tce
{
	namespace geom
	{
		template class BrushConverter<polylfList>;

		//template class BrushConverter<polyldList>;

		template class BrushConverter<BSPFacePolylfVector, BSPPolyPlaneAccess<Plane3f, BSPFacePolylfVector::polyType> >;

		template class BrushConverter<BSPFacePolyvfVector, BSPPolyPlaneAccess<Plane3f, BSPFacePolyvfVector::polyType> >;
	}
}
