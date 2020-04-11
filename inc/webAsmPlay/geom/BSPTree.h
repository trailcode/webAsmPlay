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
\copyright 2020
*/

// Original code from a course at the GameInstitute. 

#pragma once

#include <webAsmPlay/geom/CSG.h>

//-----------------------------------------------------------------------------
// Typedef Structures
//-----------------------------------------------------------------------------
typedef struct tagPLANE
{
	glm::fvec3 PointOnPlane;				// Point On Plane
	glm::fvec3 Normal;						// Plane Normal
} PLANE;

typedef struct BSPPOLYGON					// BSP Polygon (Typecast Safe with POLYGON)
{
	D3DVERTEX*	Vertices;					// Actual Vertex Data
	uint16_t*		Indices;					// Actual Indices for this poly
	uint16_t		VertexCount;				// Number Of Vertices
	uint16_t		IndexCount;					// Number of Indices
	glm::fvec3	Normal;						// Faces Normal
	int			TextureIndex;				// Stores the texture index.
	BSPPOLYGON*	NextPoly;					// Linked List to next poly in chain.
	uint8_t		UsedAsSplitter;				// Has this poly already been used
} BSPPOLYGON;

typedef struct
{
	long		StartPoly;					// Indices into polygon array
	long		EndPoly;					// End Amount of poly's to read
} LEAF;

typedef struct
{
	uint8_t		IsLeaf;						// Does this node point to a leaf ??
	long		Plane;						// Index into Plane Array
	long		Front;						// Front Child (Another node or leaf if IsLeaf = 1)
	long		Back;						// Back Child (Node Only, will be -1 if NULL)
} NODE;


//-----------------------------------------------------------------------------
// Constants (Various constants for BSP Compilation / rendering )
//-----------------------------------------------------------------------------
#define			CP_FRONT			1		// In Front of Plane
#define			CP_BACK				2		// Behind Plane
#define			CP_ONPLANE			3		// Co Planar
#define			CP_SPANNING			4		// Spanning the plane

//-----------------------------------------------------------------------------
// Name : CBSPTree ()
// Desc : BSP Class used for compilation, clipping / rendering the trees
//-----------------------------------------------------------------------------
class CBSPTree 
{
public:
				CBSPTree();
				~CBSPTree();
	//------------------------------
	//     Function Prototypes 
	//------------------------------
	bool		ClipTree( long CurrentNode, long *Polygons, long pPolyCount, CBSPTree *BSPTree, bool ClipSolid, bool RemoveCoPlanar);
	void		InitPolygons( BRUSH *brush, long BrushCount, bool BSPCollect );	
	void		RenderBSPPolys( int BrushIndex );
	void		InvertPolys();

	BSPPOLYGON	*AddPolygonEX( BSPPOLYGON*Poly );
	CBSPTree	*CopyTree();

	//------------------------------
	//      Public Variables
	//------------------------------
	BSPPOLYGON*		Polygons;					// Actual Polygon Data
	bool			FatalError;					// Did a fatal error occur ??
	bool*			PolygonDeleted;				// Temp Storage to check for Deleted Polys
	BRUSH*			BrushBase;					// Which brush this is attatched to.
	long			PolyCount;					// Number Of polygons
	long			RootNode;					// Index to the Root Node.
	BSPPOLYGON		*PolygonList;				// Entry linked list poly.
	PLANE*			Planes;						// Node's Plane Data
	NODE*			Nodes;						// Tree Nodes
	LEAF*			Leaves;						// Tree's Leaves
	long			NodeCount;					// Number Of Nodes
	long			PlaneCount;					// Number Of Planes
	long			LeafCount;					// Number Of Leaves

	bool			IsDirty;					// Has the BSP been modified since it was built ?
	bool			m_bMisc;					// Miscellaneous bool we can use for many things

private:
	//------------------------------
	//     Function Prototypes 
	//------------------------------
	
	
	bool		Get_Intersect( glm::fvec3 *linestart, glm::fvec3 *lineend, glm::fvec3 *vertex, glm::fvec3 *normal, glm::fvec3 * intersection, float *percentage );
	void		SplitPolygon(BSPPOLYGON *Poly,PLANE *Plane,BSPPOLYGON *FrontSplit,BSPPOLYGON *BackSplit);
	long		SelectBestSplitter( BSPPOLYGON *PolyList, long CurrentNode );
	void		BuildBSPTree( long CurrentNode, BSPPOLYGON *PolyList );
	
	int			ClassifyPoly( PLANE *Plane, BSPPOLYGON * Poly );
	int			ClassifyPoint( glm::fvec3 *pos, PLANE *Plane ) ;
	int			AllocAddPlane( );
	int			AllocAddPoly( );
	int			AllocAddNode( );
	int			AllocAddLeaf( );
	void		KillTree();
	void		KillPolys();

	BSPPOLYGON	*AddPolygonSplit (BSPPOLYGON *Parent, POLYGON    *Face );
	BSPPOLYGON	*AddPolygon      (BSPPOLYGON *Parent, POLYGON    *Face );
	BSPPOLYGON  *AddBSPPolygon   (BSPPOLYGON *Parent, BSPPOLYGON *Poly );

};



