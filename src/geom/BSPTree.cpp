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

#include <webAsmPlay/geom/BSPTree.h>

using namespace std;
using namespace glm;

//-----------------------------------------------------------------------------
// Name : CBSPTree () (Constructor)
// Desc : Initialises Everything We Need
//-----------------------------------------------------------------------------
CBSPTree::CBSPTree()
{
	FatalError		= false;
	BrushBase		= nullptr;
	Polygons		= nullptr;
	Planes			= nullptr;
	Nodes			= nullptr;
	Leaves			= nullptr;
	PolygonDeleted	= nullptr;
	RootNode		= -1;
	NodeCount		= 0;
	PolyCount		= 0;
	PlaneCount		= 0;
	LeafCount		= 0;
}

//-----------------------------------------------------------------------------
// Name : ~CBSPTree () (Destructor)
// Desc : Destroys Everything We No Longer Need
//-----------------------------------------------------------------------------
CBSPTree::~CBSPTree()
{
	KillTree();
	KillPolys();
}

//-----------------------------------------------------------------------------
// Name : KillTree ()
// Desc : Destroys Everything We No Longer Need
//-----------------------------------------------------------------------------
void CBSPTree::KillTree()
{
	
	// Kill The Leaf Data
	if ( Leaves )	{ free( Leaves );	Leaves		= nullptr; }
	// Free the planes
	if ( Planes )	{ free( Planes );	Planes		= nullptr; }
	// Free the Nodes
	if ( Nodes )	{ free( Nodes );	Nodes		= nullptr; }
	
	// Clean Up
	FatalError		= false;
	BrushBase		= nullptr;
	RootNode		= -1;
	NodeCount		= 0;
	PlaneCount		= 0;
	LeafCount		= 0;
}

//-----------------------------------------------------------------------------
// Name : KillPolys ()
// Desc : Destroys the polygons we created in the array.
//-----------------------------------------------------------------------------
void CBSPTree::KillPolys()
{
	// Kill the polygon data
	if ( Polygons ) { 
		// We need to free up our vertices and indices
		// allocated memory.
		for ( int i = 0; i < PolyCount; i++ ) {
			if ( Polygons[i].Vertices ) { free(Polygons[i].Vertices); Polygons[i].Vertices = nullptr; }
			if ( Polygons[i].Indices )  { free(Polygons[i].Indices);  Polygons[i].Indices  = nullptr; }
		} // End For I
		// Free the polygon data
		free( Polygons ); Polygons	= nullptr;
	} // End If Polygons != nullptr
	
	// Free PolygonDeleted Data
	if ( PolygonDeleted )	{ free( PolygonDeleted );	PolygonDeleted = nullptr; }

	PolyCount		= 0;
}

//-----------------------------------------------------------------------------
// Name : InitPolygons ()
// Desc : Add's all the required poly's to the tree to start compilation
//        We basically compile using the brushes from the BRUSH passed
//        up to the BrushCount also passed.
//-----------------------------------------------------------------------------
void CBSPTree::InitPolygons( BRUSH *brush, long BrushCount, bool BSPCollect )
{
	FatalError = false;
	
	KillTree();				// Free our previous tree
	
	BrushBase = brush;
	
	PolygonList = nullptr;
	
	// If we don't want to collect these polygons from
	// a bsp tree, we will collect the polygons from
	// the brush itself

	BSPPOLYGON *Child = nullptr;

	if ( !BSPCollect ) {
		for ( size_t f = 0; f < brush->FaceCount; f++ ) {
			Child = AddPolygon( Child, &brush->Faces[f] );
			//Child = AddPolygonSplit( Child, &brush->Faces[f] );
		} // End For Brush Faces
	} else {
		for ( int b = 0; b < BrushCount; b++ ) {
			CBSPTree * MiniBSP = (CBSPTree*)brush[b].BSPTree;
			if ( MiniBSP == nullptr ) continue;
			for ( int f = 0; f < MiniBSP->PolyCount; f++ ) {
				if (!MiniBSP->PolygonDeleted[f]) {
					if ( MiniBSP->Polygons[f].VertexCount < 3 ) continue;
					Child = AddBSPPolygon( Child, &MiniBSP->Polygons[f] );
				} // End If poly is not deleted
			} // End For Brush Faces
		} // Next Brush
	} // End If BrushCount == 1

	
	// Before we build the tree we must free our old poly array
	// NOTE: the reason we kill the polys separately is because
	// this initpolygons may actually be required to collect
	// polys from itself when collecting polys from a bsp tree.
	KillPolys();
	
	// Add the root node.
	RootNode = AllocAddNode();
	// Now compile the actual tree from this data
	BuildBSPTree( RootNode, PolygonList );

	// Flag as Clean
	IsDirty = false;

}

//-----------------------------------------------------------------------------
// Name : AddBSPPolygon ()
// Desc : Extended Add Polygon. This function add's a simple Poly copied from
//        a BSP Tree Polygon to the LINKED LIST
//-----------------------------------------------------------------------------
BSPPOLYGON *CBSPTree::AddBSPPolygon( BSPPOLYGON *Parent, BSPPOLYGON*Poly )
{
	BSPPOLYGON *Child		= new BSPPOLYGON;
	Child->IndexCount		= Poly->IndexCount;
	Child->VertexCount		= Poly->VertexCount;
	Child->Normal			= Poly->Normal;
	Child->TextureIndex		= Poly->TextureIndex;
	Child->UsedAsSplitter	= 0;
	Child->NextPoly			= nullptr;

	Child->Vertices			= (D3DVERTEX*)malloc(Poly->VertexCount * sizeof(D3DVERTEX));
	Child->Indices			= (uint16_t*)malloc(Poly->IndexCount * sizeof(uint16_t));

	memcpy( &Child->Vertices[0], &Poly->Vertices[0], Poly->VertexCount * sizeof(D3DVERTEX));
	memcpy( &Child->Indices[0], &Poly->Indices[0], Poly->IndexCount * sizeof(uint16_t));
	
	if ( Parent != nullptr)		Parent->NextPoly	= Child;
	if ( PolygonList == nullptr)	PolygonList			= Child;
	return Child;
}

//-----------------------------------------------------------------------------
// Name : AddPolygon ()
// Desc : Add's a polygon from the Brush face passed
//-----------------------------------------------------------------------------
BSPPOLYGON *CBSPTree::AddPolygon(BSPPOLYGON *Parent, POLYGON *Face )
{
	BSPPOLYGON *Child		= new BSPPOLYGON;
	Child->NextPoly			= nullptr;
	Child->UsedAsSplitter	= 0;

	// Copy Poly Data
	Child->VertexCount		= Face->VertexCount;
	Child->IndexCount		= Face->IndexCount;
	Child->Normal			= Face->Normal;
	Child->TextureIndex		= Face->TextureIndex;
	
	// Reserve space for Vertex and Index Lists
	Child->Vertices			= (D3DVERTEX*)malloc( Child->VertexCount * sizeof(D3DVERTEX));
	Child->Indices			= (uint16_t*)malloc( Child->IndexCount * sizeof(uint16_t));

	// Copy the Vertex Data
	for ( int i = 0; i < Face->VertexCount; i++) {
		Child->Vertices[i] = Face->Vertices[i];
		/*
		if (g_bSafeTransform) {
			D3DXfvec3TransformCoord( (fvec3*)&Child->Vertices[i], (fvec3*)&Child->Vertices[i], &BrushBase->Matrix );
		} // End If Safe Transform
		*/
	} // Next Vertex

	// Copy the Index Data
	for ( size_t i = 0; i < Face->IndexCount; i++) Child->Indices[i] = Face->Indices[i];

	// Set parent's child linked list variable
	if (PolygonList==nullptr) PolygonList = Child;
	if (Parent!=nullptr) Parent->NextPoly = Child;
	return Child;
}

//-----------------------------------------------------------------------------
// Name : AddPolygonSplit ()
// Desc : Add's a polygon from the Brush face passed
//        This function also does some pretty nifty Illegal geometry fixes =)
//        Which fix problems resulting from accumulation errors. Often with
//        Multi tri polygons, accumulation errors, can cause the Normal
//        to become mangled (i.e it no longer matches all the tri's of which it
//        is describing), so this function does a quick pass to check if all
//        neighbouring tri's share the same normal, if not, the primitive is 
//        split into multiple polygons as sets of neighbouring tri's which 
//        share the same normal
//-----------------------------------------------------------------------------
BSPPOLYGON *CBSPTree::AddPolygonSplit(BSPPOLYGON *Parent, POLYGON *Face)
{
    fvec3 PreviousNorm, CurrentNormal;
	uint16_t		IndexOn			= 0, IndexOn2 = 0;
	bool		PolyFolded		= false;
	BSPPOLYGON	*pParent		= Parent, *RetVal = nullptr;

    PreviousNorm = CalculatePolyNormal(Face->Vertices[Face->Indices[0]],
		                               Face->Vertices[Face->Indices[1]],
									   Face->Vertices[Face->Indices[Face->IndexCount - 1]],
									   nullptr);

	// First of all, just do a trivial test to see if the poly is folded at all
	// if it isn't then just add the polygon in it's current state.
    for (int lloop = 0;  lloop < (Face->IndexCount / 3); lloop++ ) {
        CurrentNormal = CalculatePolyNormal(Face->Vertices[Face->Indices[lloop * 3]], 
											Face->Vertices[Face->Indices[(lloop * 3) + 1]], 
											Face->Vertices[Face->Indices[(lloop * 3) + 2]],
											nullptr);
		if ( !EpsilonCompareVector( CurrentNormal, PreviousNorm ) ) { PolyFolded = true; break; }
    } // Next lloop

	// If no polys were folded, then just add the damn poly =)
    if ( PolyFolded == false ) {		
		BSPPOLYGON *Child		= new BSPPOLYGON;
		Child->NextPoly			= nullptr;
		Child->UsedAsSplitter	= 0;


		// Copy Poly Data
		Child->VertexCount		= Face->VertexCount;
		Child->IndexCount		= Face->IndexCount;
		Child->Normal			= Face->Normal;
		Child->TextureIndex		= Face->TextureIndex;		
		
		// Reserve space for Vertex and Index Lists
		Child->Vertices			= (D3DVERTEX*)malloc( Child->VertexCount * sizeof(D3DVERTEX));
		Child->Indices			= (uint16_t*)malloc( Child->IndexCount * sizeof(uint16_t));

		// Copy the Vertex Data
		memcpy( Child->Vertices, Face->Vertices, Child->VertexCount * sizeof(D3DVERTEX));
		// Copy the Index Data
		memcpy( Child->Indices, Face->Indices, Child->IndexCount * sizeof(uint16_t));
		
		// Set parent's child linked list variable
		if (PolygonList==nullptr) PolygonList = Child;
		if (Parent!=nullptr) Parent->NextPoly = Child;

		return Child;
	}

	// If the polygon was totally co planar then
	// loop round and split the polygon into polys made up of
	// tri's which all share the same normal. If this step is not
	// performed, BSP compilation sometimes fails, because of the 
	// fact that Poly's and the planes no longer match up.
    PreviousNorm = fvec3( 0.0f, 0.0f, 0.0f);
    for ( size_t lloop = 0;  lloop < (Face->IndexCount / 3); lloop++ ) {
        CurrentNormal = CalculatePolyNormal(Face->Vertices[Face->Indices[lloop * 3]], 
											Face->Vertices[Face->Indices[(lloop * 3) + 1]], 
											Face->Vertices[Face->Indices[(lloop * 3) + 2]],
											nullptr);
		if ( !EpsilonCompareVector( CurrentNormal, PreviousNorm ) ) { 
            
			BSPPOLYGON *Child = new BSPPOLYGON;
			
			if (PolygonList==nullptr) PolygonList = Child;
            if (pParent!=nullptr) pParent->NextPoly = Child;

			Child->NextPoly			= nullptr;
			Child->UsedAsSplitter	= 0;

			// Create Poly Data
			Child->Normal			= CurrentNormal;
			Child->TextureIndex		= Face->TextureIndex;
		
			Child->VertexCount		= 3;
			Child->IndexCount		= 3;
			
			// Reserve space for Vertex and Index Lists
			Child->Vertices			= (D3DVERTEX*)malloc( Child->VertexCount * sizeof(D3DVERTEX));
			Child->Indices			= (uint16_t*)malloc( Child->IndexCount * sizeof(uint16_t));

            Child->Vertices[0] = Face->Vertices[Face->Indices[(lloop * 3)]];
            Child->Vertices[1] = Face->Vertices[Face->Indices[(lloop * 3) + 1]];
            Child->Vertices[2] = Face->Vertices[Face->Indices[(lloop * 3) + 2]];

            Child->Indices[0] = 0;
            Child->Indices[1] = 1;
            Child->Indices[2] = 2;
			
			// To Let us know what index we are on
            IndexOn = 3; IndexOn2 = 2;

			pParent = Child;

        } else { // The current tri was not folded
            pParent->VertexCount++;
			pParent->Vertices				= (D3DVERTEX*)realloc( pParent->Vertices, pParent->VertexCount * sizeof(D3DVERTEX));
			pParent->Vertices[pParent->VertexCount - 1] = Face->Vertices[Face->Indices[(lloop * 3) + 2]];
            
            pParent->IndexCount+= 3;
            pParent->Indices				= (uint16_t*)realloc( pParent->Indices, pParent->IndexCount * sizeof(uint16_t));

            pParent->Indices[IndexOn]		= 0;
            pParent->Indices[IndexOn + 1]	= IndexOn2;
            pParent->Indices[IndexOn + 2]	= IndexOn2 + 1;
            IndexOn2++; IndexOn+=3;
        } // End If Normals are different
		PreviousNorm = CurrentNormal;
    } // Next lloop
    
	return pParent;
}

//-----------------------------------------------------------------------------
// Name : BuildBSPTree () ( Recursive )
// Desc : Build's the entire BSP Tree using the already initialised poly data
//-----------------------------------------------------------------------------
void CBSPTree::BuildBSPTree( long CurrentNode, BSPPOLYGON *PolyList )
{

	fvec3 a, b; float result;
	BSPPOLYGON *polyTest=nullptr, *FrontList=nullptr, *BackList=nullptr;
	BSPPOLYGON *NextPolygon=nullptr, *FrontSplit=nullptr, *BackSplit=nullptr;
	
	if ( FatalError == true) return;

	// First of all we need to Select the best splitting
	// plane from the remaining polygon list.
	Nodes[CurrentNode].Plane = SelectBestSplitter( PolyList, CurrentNode );
	
	if ( FatalError == true) return;

	// Store the poly list (we need to use the original later)
	polyTest = PolyList;

	while ( polyTest!=nullptr ) {
		// Remember to store because 
		// polytest->Next will be altered
		NextPolygon = polyTest->NextPoly;

		switch ( ClassifyPoly( &Planes[Nodes[CurrentNode].Plane ], polyTest ) )
		{
			case CP_ONPLANE:
				// If the poly end's up on the plane, wee need to pass it
				// down the side the plane is facing, so we do a quick test
				// and pass it down the appropriate side.
				a = Planes[Nodes[CurrentNode].Plane].Normal;
				b = polyTest->Normal;	
				result = (float)fabs( ( a.x - b.x ) + ( a.y - b.y ) + ( a.z - b.z ) );
				if ( result < 0.1f ) {	
					polyTest->NextPoly	= FrontList;
					FrontList			= polyTest;
				} else {
					polyTest->NextPoly	= BackList;
					BackList			= polyTest;	
				} // End If Plane Facing
				break;
			case CP_FRONT:
				// Pass the poly straight down the front list.
				polyTest->NextPoly		= FrontList;
				FrontList				= polyTest;		
				break;
			case CP_BACK:
				// Pass the poly straight down the back list.
				polyTest->NextPoly		= BackList;
				BackList				= polyTest;	
				break;
			case CP_SPANNING:
				// If Poly is spanning the plane we need to split
				// it and pass each fragment down the appropriate side.
				FrontSplit = new BSPPOLYGON;
				BackSplit  = new BSPPOLYGON;
				memset( FrontSplit, 0, sizeof(BSPPOLYGON) );
				memset( BackSplit , 0, sizeof(BSPPOLYGON) );
				
				// Split the polygon
				SplitPolygon( polyTest, &Planes[Nodes[CurrentNode].Plane], FrontSplit, BackSplit );
    
				FrontSplit->UsedAsSplitter = polyTest->UsedAsSplitter;
				BackSplit->UsedAsSplitter  = polyTest->UsedAsSplitter;
				
				free ( polyTest->Vertices );
				free ( polyTest->Indices );
				delete polyTest; polyTest = nullptr;
	
				// Copy fragements to the front/back list
				FrontSplit->NextPoly = FrontList;
				FrontList			 = FrontSplit;
				BackSplit->NextPoly	 = BackList;
				BackList			 = BackSplit;

				break;
			default:

				break;
		} //switch

		polyTest = NextPolygon;
	}// End while loop
   
	// Count the splitters remaining
	// In this list
	int SplitterCount = 0;
	BSPPOLYGON *tempf	  = FrontList;
	while ( tempf != nullptr) {
		if ( tempf->UsedAsSplitter == 0) SplitterCount++;
		tempf = tempf->NextPoly;
	} 

	// If there are no splitters remaining
	// We can go ahead and add the leaf.
	if ( SplitterCount == 0) {
		BSPPOLYGON *Iterator = FrontList;
		BSPPOLYGON *Temp;
		
		// Add a new leaf
		AllocAddLeaf();
		Leaves[LeafCount-1].StartPoly = PolyCount;
		while (Iterator!=nullptr) {
			Polygons[ AllocAddPoly() ] = *Iterator;
			Temp = Iterator;
			Iterator = Iterator->NextPoly;
			// we use the normal delete operator here because we dont want to delete the actual vertexLists and index lists
			// because we are still referenceing them in the Polygon Array
			delete Temp;
		} // End While
		Leaves[LeafCount-1].EndPoly = PolyCount;
		Nodes[CurrentNode].Front  = LeafCount-1;
		Nodes[CurrentNode].IsLeaf = 1;
	} else {
		// Otherwise create a new node, and
		// push the front list down the tree.
		Nodes[CurrentNode].IsLeaf = 0;
		Nodes[CurrentNode].Front =AllocAddNode();
		BuildBSPTree( NodeCount-1, FrontList );  
	} // End If No Splitters Left

	// If the back list is empty
	if (BackList == nullptr) {
		Nodes[CurrentNode].Back = -1;
	} else {
		Nodes[CurrentNode].Back = AllocAddNode();
		BuildBSPTree( NodeCount-1, BackList );
	}  // End If BackList Empty.
  
}// end function

//-----------------------------------------------------------------------------
// Name : AllocAddNode ()
// Desc : Reallocates memory for the NODE array and returns the last added.
//-----------------------------------------------------------------------------
int CBSPTree::AllocAddNode( )
{
	NodeCount++;
	Nodes = (NODE*)realloc( Nodes, NodeCount * sizeof(NODE) );
	Nodes[NodeCount-1].IsLeaf	= 0;
	Nodes[NodeCount-1].Plane	= -1;
	Nodes[NodeCount-1].Front	= -1;
	Nodes[NodeCount-1].Back		= -1;
	return NodeCount-1;
}

//-----------------------------------------------------------------------------
// Name : AllocAddLeaf ()
// Desc : Reallocates memory for the LEAF array and returns the last added.
//-----------------------------------------------------------------------------
int CBSPTree::AllocAddLeaf( )
{
	LeafCount++;
	Leaves = (LEAF*)realloc( Leaves, LeafCount * sizeof(LEAF) );
	Leaves[LeafCount-1].StartPoly		= -1;
	Leaves[LeafCount-1].EndPoly			= -1;
	return LeafCount-1;
}

//-----------------------------------------------------------------------------
// Name : AllocAddPoly ()
// Desc : Reallocates memory for the POLY array and returns the last added.
//-----------------------------------------------------------------------------
int CBSPTree::AllocAddPoly( )
{
	PolyCount++;
	Polygons		= (BSPPOLYGON*)realloc( Polygons, PolyCount * sizeof(BSPPOLYGON) );
	PolygonDeleted  = (bool*)realloc( PolygonDeleted, PolyCount * sizeof(bool) );
	PolygonDeleted[PolyCount-1]				= false;
	Polygons[PolyCount-1].IndexCount		= 0;
	Polygons[PolyCount-1].Indices			= nullptr;
	Polygons[PolyCount-1].VertexCount		= 0;
	Polygons[PolyCount-1].Vertices			= nullptr;
	Polygons[PolyCount-1].Normal			= fvec3( 0.0f, 0.0f, 0.0f );
	Polygons[PolyCount-1].NextPoly			= nullptr;
	Polygons[PolyCount-1].UsedAsSplitter	= 0;
	return PolyCount-1;
}

//-----------------------------------------------------------------------------
// Name : AllocAddPlane ()
// Desc : Reallocates memory for the PLANE array and returns the last added.
//-----------------------------------------------------------------------------
int CBSPTree::AllocAddPlane( )
{
	PlaneCount++;
	Planes = (PLANE*)realloc( Planes, PlaneCount * sizeof(PLANE) );
	Planes[PlaneCount-1].PointOnPlane	= fvec3( 0.0f, 0.0f, 0.0f );
	Planes[PlaneCount-1].Normal			= fvec3( 0.0f, 0.0f, 0.0f );
	return PlaneCount-1;
}

//-----------------------------------------------------------------------------
// Name : SelectBestSplitter ()
// Desc : Picks the splitter which most closely matches the splitting heuristic
//-----------------------------------------------------------------------------
long CBSPTree::SelectBestSplitter( BSPPOLYGON *PolyList, long CurrentNode )
{
	BSPPOLYGON *Splitter = PolyList, *CurrentPoly = nullptr, *SelectedPoly = nullptr;
	size_t   BestScore = 1000000;

	// Traverse the Poly Linked List
	while ( Splitter != nullptr ) {
		// If this has not been used as a splitter then
		if ( Splitter->UsedAsSplitter != 1 ) {
			// Create the testing splitter plane
			PLANE SplittersPlane;
			SplittersPlane.Normal		= Splitter->Normal;
			SplittersPlane.PointOnPlane	= *(fvec3*)&Splitter->Vertices[0];
			
			CurrentPoly					= PolyList;
			size_t score, splits, backfaces, frontfaces;
			score = splits = backfaces = frontfaces = 0;
			
			// Test against the other poly's
			// and count the score.
			while ( CurrentPoly != nullptr ) {
				int result = ClassifyPoly( &SplittersPlane, CurrentPoly );
				switch ( result ) {
					case CP_ONPLANE:
						break;
					case CP_FRONT:
						frontfaces++;
						break;
					case CP_BACK:
						backfaces++;
						break;
					case CP_SPANNING:
						splits++;
						break;
					default:
						break;
				}// switch
				CurrentPoly = CurrentPoly->NextPoly;
			} // end while current poly
			
			// Tally the score (modify the splits * n)
			score = abs( frontfaces - backfaces ) + ( splits * 3 );
			
			if ( score < BestScore) {
				BestScore	 = score;
				SelectedPoly = Splitter;
			}
  
		}// end if this splitter has not been used yet		
		Splitter = Splitter->NextPoly;
	}// end while splitter == nullptr	

	if ( SelectedPoly == nullptr ) {
		FatalError = true;
		return -1;
	}

	SelectedPoly->UsedAsSplitter = 1;

	// Return the selected poly's plane
	AllocAddPlane();
	Planes[PlaneCount-1].PointOnPlane = *((fvec3 *)&SelectedPoly->Vertices[0]);
	Planes[PlaneCount-1].Normal		  = SelectedPoly->Normal;
	return ( PlaneCount-1 );
}

//-----------------------------------------------------------------------------
// Name : ClassifyPoly ()
// Desc : Given a plane and a poly, returns the poly's position relative
//        to the plane itself (i.e Front/Back/On/Spanning)
//-----------------------------------------------------------------------------
int CBSPTree::ClassifyPoly( PLANE *Plane, BSPPOLYGON * Poly )
{
	int			Infront = 0, Behind = 0, OnPlane=0;
	float		result  = 0;
	fvec3	*vec1   = &Plane->PointOnPlane;

	// Loop round each vertex
	for ( int a = 0; a < Poly->VertexCount; a++ ) {
		fvec3 *vec2 = (fvec3*)&Poly->Vertices[a];
		fvec3 Direction = (*vec1) - (*vec2);
		result = dot( Direction, Plane->Normal );

		// Check the position
		if (result>g_EPSILON) {
			Behind++;
		} else if (result<-g_EPSILON) {
			Infront++;
		} else {
			OnPlane++;
			Infront++;
			Behind++;
		}
	} // End For Each Vertex

	if ( OnPlane == Poly->VertexCount ) return CP_ONPLANE;
	if ( Behind  == Poly->VertexCount ) return CP_BACK;
	if ( Infront == Poly->VertexCount ) return CP_FRONT;
	return CP_SPANNING;
}

//-----------------------------------------------------------------------------
// Name : ClassifyPoint ()
// Desc : Given a plane and a point, returns the points's position relative
//        to the plane itself (i.e Front/Back/On)
//-----------------------------------------------------------------------------
int CBSPTree::ClassifyPoint( fvec3 *pos, PLANE *Plane ) 
{
	float result;
	fvec3 *vec1		= &Plane->PointOnPlane;
	fvec3 Direction	= (*vec1) - (*pos);
	result					= dot( Direction, Plane->Normal );
	if ( result < -g_EPSILON ) return CP_FRONT;
	if ( result > g_EPSILON  ) return CP_BACK;
	return CP_ONPLANE;
}

//-----------------------------------------------------------------------------
// Name : SplitPolygon ()
// Desc : Splits the passed polygon against the plane provided, the
//        fragments are then passed back through the Front/BackSplit
//-----------------------------------------------------------------------------
void CBSPTree::SplitPolygon(BSPPOLYGON *Poly,PLANE *Plane,BSPPOLYGON *FrontSplit,BSPPOLYGON *BackSplit)
{
	// 50 is used here, as we should never
	// really have more points on a portal than this.
	D3DVERTEX		FrontList[50];
	D3DVERTEX		BackList[50];
	int				FrontCounter = 0;
	int				BackCounter = 0;

	int				PointLocation[50];

	int				InFront = 0, Behind = 0, OnPlane = 0;
	int				CurrentVertex = 0, Location = 0;

	// Determine each points location relative to the plane.
	for ( int i = 0; i < Poly->VertexCount; i++)	{
		Location = ClassifyPoint((fvec3*)&Poly->Vertices[i], Plane);
		if (Location == CP_FRONT )
			InFront++;
		else if (Location == CP_BACK )
			Behind++;
		else
			OnPlane++;

		PointLocation[i] = Location;
	} // Next Vertex

	if (!InFront) {
		memcpy(BackList, Poly->Vertices, Poly->VertexCount * sizeof(D3DVERTEX));
		BackCounter = Poly->VertexCount;
	}

	if (!Behind) {
		memcpy(FrontList, Poly->Vertices, Poly->VertexCount * sizeof(D3DVERTEX));
		FrontCounter = Poly->VertexCount;
	}

	if (InFront && Behind) {
		for ( size_t i = 0; i < Poly->VertexCount; i++) {
			// Store Current vertex remembering to MOD with number of vertices.
			CurrentVertex = (i+1) % Poly->VertexCount;

			if (PointLocation[i] == CP_ONPLANE ) {
				FrontList[FrontCounter] = Poly->Vertices[i];
				FrontCounter++;
				BackList[BackCounter] = Poly->Vertices[i];
				BackCounter++;
				continue; // Skip to next vertex
			}
			if (PointLocation[i] == CP_FRONT ) {
				FrontList[FrontCounter] = Poly->Vertices[i];
				FrontCounter++;
			} else {
				BackList[BackCounter] = Poly->Vertices[i];
				BackCounter++;
			}
			
			// If the next vertex is not causing us to span the plane then continue
			if (PointLocation[CurrentVertex] == CP_ONPLANE || PointLocation[CurrentVertex] == PointLocation[i]) continue;
			
			// Otherwise create the new vertex
			fvec3 IntersectPoint;
			float		percent;

			Get_Intersect( (fvec3*)&Poly->Vertices[i], (fvec3*)&Poly->Vertices[CurrentVertex], &Plane->PointOnPlane, &Plane->Normal, &IntersectPoint, &percent );

			// create new vertex and calculate new texture coordinate
			D3DVERTEX copy;
			float deltax	= Poly->Vertices[CurrentVertex].tu - Poly->Vertices[i].tu;
			float deltay	= Poly->Vertices[CurrentVertex].tv - Poly->Vertices[i].tv;
			float texx		= Poly->Vertices[i].tu + ( deltax * percent );
			float texy		= Poly->Vertices[i].tv + ( deltay * percent );
			copy.x			= IntersectPoint.x; copy.y = IntersectPoint.y; copy.z = IntersectPoint.z;
			copy.tu			= texx; copy.tv = texy;
	

			BackList[BackCounter++]		= copy;			
			FrontList[FrontCounter++]	= copy;
		}
	}

	//OK THEN LETS BUILD THESE TWO POLYGONAL BAD BOYS

	// Reserve Memory for Front and Back Vertex Lists 
	FrontSplit->Vertices		 = (D3DVERTEX*)malloc(FrontCounter * sizeof(D3DVERTEX));
	BackSplit->Vertices			 = (D3DVERTEX*)malloc(BackCounter * sizeof(D3DVERTEX));

	FrontSplit->VertexCount		 = 0;
	BackSplit->VertexCount		 = 0;

	// Copy over the vertices into the new polys
	FrontSplit->VertexCount		 = FrontCounter;
	memcpy(FrontSplit->Vertices, FrontList, FrontCounter * sizeof(D3DVERTEX));
	
	BackSplit->VertexCount		= BackCounter;
	memcpy(BackSplit->Vertices, BackList, BackCounter * sizeof(D3DVERTEX));


	BackSplit->IndexCount		= ( BackSplit->VertexCount  - 2 ) * 3;
	FrontSplit->IndexCount		= ( FrontSplit->VertexCount - 2 ) * 3;
	
	// Reserve Memory for Front and Back Index Lists 
	BackSplit->Indices			= (uint16_t*)malloc(BackSplit->IndexCount * sizeof(uint16_t));
	FrontSplit->Indices			= (uint16_t*)malloc(FrontSplit->IndexCount * sizeof(uint16_t));
	
	// Fill in the Indices
	short IndxBase;
	for ( short loop = 0, v1 = 1, v2 = 2; loop < FrontSplit->IndexCount/3; loop++, v1 = v2, v2++) {
		IndxBase = loop * 3;
		FrontSplit->Indices[ IndxBase    ] =  0;
		FrontSplit->Indices[ IndxBase + 1] = v1;
		FrontSplit->Indices[ IndxBase + 2] = v2;
	} // Next Tri

	for ( size_t loop = 0, v1 = 1, v2 = 2; loop < BackSplit->IndexCount/3; loop++, v1 = v2, v2++) {
		IndxBase = loop * 3;
		BackSplit->Indices[ IndxBase    ] =  0;
		BackSplit->Indices[ IndxBase + 1] = v1;
		BackSplit->Indices[ IndxBase + 2] = v2;
	} // Next Tri

	// Copy Extra Values
	FrontSplit->Normal			= Poly->Normal;
	BackSplit->Normal			= Poly->Normal;
	FrontSplit->TextureIndex	= Poly->TextureIndex;
	BackSplit->TextureIndex		= Poly->TextureIndex;

	// Copy over to vertex normals
	for ( size_t i = 0; i <  BackSplit->VertexCount; i++ )  BackSplit->Vertices [i].Normal =  BackSplit->Normal;
	for ( size_t i = 0; i < FrontSplit->VertexCount; i++ )  FrontSplit->Vertices[i].Normal = FrontSplit->Normal;

}

//-----------------------------------------------------------------------------
// Name : GetIntersect ()
// Desc : Returns the Intersection point between a ray and a  plane
//-----------------------------------------------------------------------------
bool CBSPTree::Get_Intersect( fvec3 *linestart, fvec3 *lineend, fvec3 *vertex, fvec3 *normal, fvec3 * intersection, float *percentage )
{
	fvec3 direction, L1;
	float	    linelength, dist_from_plane;

	direction.x = lineend->x - linestart->x;
	direction.y = lineend->y - linestart->y;
	direction.z = lineend->z - linestart->z;

	linelength = dot( direction, *normal );
	if (fabsf( linelength ) < g_EPSILON ) return false; 

	L1.x = vertex->x - linestart->x;
	L1.y = vertex->y - linestart->y;
	L1.z = vertex->z - linestart->z;

	dist_from_plane = dot( L1, *normal );

	// How far from Linestart , intersection is as a percentage of 0 to 1 
	*percentage = dist_from_plane / linelength; 

	// The plane is behind the start of the line or
	// The line does not reach the plane
	if ( *percentage < 0.0f || *percentage > 1.0f ) return false;

	// add the percentage of the line to line start
	intersection->x = linestart->x + direction.x * (*percentage);
	intersection->y = linestart->y + direction.y * (*percentage);
	intersection->z = linestart->z + direction.z * (*percentage);
	return true;
}

//-----------------------------------------------------------------------------
// Name : RenderBSPPolys ()
// Desc : Simply Renders all polys in the tree
//-----------------------------------------------------------------------------
void CBSPTree::RenderBSPPolys( int BrushIndex ) 
{

	/*
	if (g_iCSGBrush == BrushIndex && !g_bWireFrame && !g_bCompileFinal && g_iCSGMode != CSG_NONE) {
		lpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	} // End if

	for ( int i = 0; i < PolyCount; i++ ) {
		if ( PolygonDeleted[i] == false ) {
			int Texture = Polygons[i].TextureIndex;
			if ( Texture != g_iLastTexture && Texture < NumberOfTextures) {
				lpDevice->SetTexture(0, lpTextureSurface[Polygons[i].TextureIndex] );
				g_iLastTexture = Polygons[i].TextureIndex;
			} // End if texture needs switching
			lpDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, Polygons[i].VertexCount, (Polygons[i].IndexCount / 3) , &Polygons[i].Indices[0], D3DFMT_INDEX16, &Polygons[i].Vertices[0], 32);
		} // If polygon is not deleted then
	} // Next Polygon
	
	if (g_iCSGBrush == BrushIndex && !g_bWireFrame && !g_bCompileFinal && g_iCSGMode != CSG_NONE) {
		lpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	} // End If
	*/
}


//-----------------------------------------------------------------------------
// Name : ClipTree ()
// Desc : Clips One BSP Tree against another. This is an EXTREMELY useful
//        function, it is used by both the CSG and HSR routines. Passing
//        false to ClipSolid will essentially Perform a carve from the
//        this tree with the tree passed... Passing true will clip away
//        Any areas of the tree inside the solid area of the tree passed.
//-----------------------------------------------------------------------------
bool CBSPTree::ClipTree( long CurrentNode, long *Polygons, long pPolyCount, CBSPTree *BSPTree, bool ClipSolid, bool RemoveCoPlanar)
{   
	fvec3 a, b;
    long *FrontList = nullptr;
    long *BackList = nullptr;
    long FSplit = -1, BSplit = -1;
    long FListCount = 0;
    long BListCount = 0;
	float result;

	// Mark the tree to be clipped as dirty
	BSPTree->IsDirty = true;

	bool FreePolyList = false;
	// If this is the first call to cliptree
	// then we must build an index list first of all.
	if ( Polygons == nullptr ) {
		FreePolyList = true;
		Polygons = new long[BSPTree->PolyCount];
		for ( int i = 0; i < BSPTree->PolyCount; i++ ) {
			Polygons[i] = i;
		} // Next Poly
		pPolyCount = BSPTree->PolyCount;
		if (pPolyCount <= 0 ) return false;
		if (PolyCount <= 0 ) return false;
	} // End If Polygons == nullptr
        
	// Pass Poly's down the tree etc.
    for ( int p = 0; p < pPolyCount; p++ ) {
        if ( BSPTree->PolygonDeleted[Polygons[p]] ) continue;
        switch ( ClassifyPoly( &Planes[Nodes[CurrentNode].Plane], &BSPTree->Polygons[Polygons[p]] ) ) 
		{
			case CP_ONPLANE:
				// Here we test the normal against the plane
				// if it is facing in the same direction then send it down the back
				// if RemoveCoPlanar is true, otherwise it gets sent down the front.
				// However, if it is facing in the opposite direction to the plane,
				// we can safely send this down the back of the tree.
				a = Planes[Nodes[CurrentNode].Plane].Normal;
				b = BSPTree->Polygons[Polygons[p]].Normal;	
				result = (float)fabs( ( a.x - b.x ) + ( a.y - b.y ) + ( a.z - b.z ) );
				if ( result < 0.1f ) {	
					if (RemoveCoPlanar) {
						BListCount++;
						BackList = (long*)realloc(BackList, BListCount * sizeof(long));
						BackList[BListCount - 1] = Polygons[p];
					} else {
						FListCount++;
						FrontList = (long*)realloc(FrontList, FListCount * sizeof(long));
						FrontList[FListCount - 1] = Polygons[p];
					}
				} else {
						BListCount++;
						BackList = (long*)realloc(BackList, BListCount * sizeof(long));
						BackList[BListCount - 1] = Polygons[p];
				} // End If Plane Facing
				break;
            case CP_FRONT:
                FListCount++;
				FrontList = (long*)realloc(FrontList, FListCount * sizeof(long));
                FrontList[FListCount - 1] = Polygons[p];
				break;
            case CP_BACK:
                BListCount++;
				BackList = (long*)realloc(BackList, BListCount * sizeof(long));
                BackList[BListCount - 1] = Polygons[p];
				break;
            case CP_SPANNING:
				FListCount++;
				FrontList = (long*)realloc(FrontList, FListCount * sizeof(long));
				FrontList[FListCount - 1] = BSPTree->AllocAddPoly();
				FSplit = FListCount - 1;
				BListCount++;
				BackList = (long*)realloc(BackList, BListCount * sizeof(long));
				BackList[BListCount - 1] = BSPTree->AllocAddPoly();
				BSplit = BListCount - 1;
				SplitPolygon( &BSPTree->Polygons[Polygons[p]], &Planes[Nodes[CurrentNode].Plane], &BSPTree->Polygons[FrontList[FSplit]], &BSPTree->Polygons[BackList[BSplit]]) ;
                
				// Since this is a coincidental pre-process on mini bsp trees
				// we don't actually need to update the leaf polys. Which is 
				// convenient =)

				BSPTree->PolygonDeleted[Polygons[p]] = true;
				break;
        } // End Switch
    } // End For p

	if ( ClipSolid ) {
		if ( Nodes[CurrentNode].Back == -1 ) {
			for ( int i = 0; i < BListCount; i++ ) {
				BSPTree->PolygonDeleted[BackList[i]] = true;
			} // End For i
			BListCount = 0;
		}
	} else {
		if ( Nodes[CurrentNode].IsLeaf == 1 ) {
			for ( int i = 0; i < FListCount; i++ ) {
				BSPTree->PolygonDeleted[FrontList[i]] = true;
			} // End For i
			FListCount = 0;
		}
	} // End If ( Clip Away the Areas in Solid Space )

	//Pass down the front tree
    if ( FListCount > 0 && Nodes[CurrentNode].IsLeaf == 0 && Nodes[CurrentNode].Front > -1) ClipTree( Nodes[CurrentNode].Front, FrontList, FListCount, BSPTree, ClipSolid, RemoveCoPlanar );
    
    //Pass down the back tree
	if ( BListCount > 0 && Nodes[CurrentNode].Back > -1) ClipTree( Nodes[CurrentNode].Back, BackList, BListCount, BSPTree, ClipSolid, RemoveCoPlanar );

	if ( FrontList ) { free( FrontList ) ; FrontList = nullptr; }
	if ( BackList ) { free( BackList ) ; BackList = nullptr; }
	// If we had to build our own PolyList, then free it up here.
	if ( FreePolyList ) delete []Polygons;
	return true;
}

//-----------------------------------------------------------------------------
// Name : InvertPolys ()
// Desc : Pretty self explanatory, simply inverts all the polys which were
//        generated by the compiler.
//-----------------------------------------------------------------------------
void CBSPTree::InvertPolys()
{
	int			Counter	= 1;
	D3DVERTEX	*TVerts = nullptr;
    for ( int i = 0; i < PolyCount; i++ ){
		if (PolygonDeleted[i]) continue;
        // create a temporary vertex array
		TVerts = new D3DVERTEX[Polygons[i].VertexCount];
		// copy vert 0 into the array
		TVerts[0] = Polygons[i].Vertices[0];
		Counter = 1;
		// copy the vertices in reverse order, into the array
        for ( int k = Polygons[i].VertexCount; k > 1; k--, Counter++ ) {
            TVerts[Counter] = Polygons[i].Vertices[k - 1];
        } // Next k
		// copy the vertices back over.
		memcpy( &Polygons[i].Vertices[0], &TVerts[0], Polygons[i].VertexCount * sizeof(D3DVERTEX));
		delete []TVerts;

		// Invert the polygons normal also
        Polygons[i].Normal = -Polygons[i].Normal;

		// copy this inverted normal into the vertex normal for d3d lighting purposes.
		for ( size_t k = 0; k < Polygons[i].VertexCount; k++ ) {
			Polygons[i].Vertices[k].Normal = Polygons[i].Normal;
		}
        
		//Recalculate Indices
		int pCount = Polygons[i].IndexCount / 3;
		for ( int lloop = 0, v1 = 1, v2 = 2; lloop < pCount; lloop ++ ){
			Polygons[i].Indices[lloop *3]		 = 0;
			Polygons[i].Indices[(lloop * 3) + 1] = v1;
			Polygons[i].Indices[(lloop * 3) + 2] = v2;
			v1 = v2;
			v2 = v2 + 1;
		}
	} // Next i
}

//-----------------------------------------------------------------------------
// Name : AddPolygonEX ()
// Desc : Extended Add Polygon. This function add's a simple Poly copied from
//        a BSP Tree Polygon TO THE POLYGON ARRAY
//-----------------------------------------------------------------------------
BSPPOLYGON *CBSPTree::AddPolygonEX( BSPPOLYGON*Poly )
{
	int Child = AllocAddPoly();
	Polygons[Child].IndexCount		= Poly->IndexCount;
	Polygons[Child].VertexCount		= Poly->VertexCount;
	Polygons[Child].Normal			= Poly->Normal;
	Polygons[Child].TextureIndex	= Poly->TextureIndex;
	Polygons[Child].UsedAsSplitter	= 0;
	Polygons[Child].NextPoly		= nullptr;

	Polygons[Child].Vertices		= (D3DVERTEX*)malloc(Poly->VertexCount * sizeof(D3DVERTEX));
	Polygons[Child].Indices			= (uint16_t*)malloc(Poly->IndexCount * sizeof(uint16_t));

	memcpy( &Polygons[Child].Vertices[0], &Poly->Vertices[0], Poly->VertexCount * sizeof(D3DVERTEX));
	memcpy( &Polygons[Child].Indices[0], &Poly->Indices[0], Poly->IndexCount * sizeof(short));

	return &Polygons[Child];

}

//-----------------------------------------------------------------------------
// Name : CopyTree ()
// Desc : Makes a copy of this tree, and places it in the DESTree passed.
//-----------------------------------------------------------------------------
CBSPTree *CBSPTree::CopyTree( )
{
	CBSPTree * DESTree = nullptr;
	if ( DESTree == nullptr) DESTree = new CBSPTree;

	// Copy Standard Vars
	DESTree->FatalError		= FatalError;
	DESTree->BrushBase		= BrushBase;
	DESTree->RootNode		= RootNode;
	DESTree->NodeCount		= NodeCount;
	DESTree->PolyCount		= PolyCount;
	DESTree->PlaneCount		= PlaneCount;
	DESTree->LeafCount		= LeafCount;
	DESTree->PolygonList	= PolygonList;

	// Create arrays suitable in destination tree
	DESTree->PolygonDeleted	= (bool*)		malloc( PolyCount	* sizeof(bool)		);
	DESTree->Polygons		= (BSPPOLYGON*)	malloc( PolyCount	* sizeof(BSPPOLYGON));
	DESTree->Planes			= (PLANE*)		malloc( PlaneCount	* sizeof(PLANE)		);
	DESTree->Nodes			= (NODE*)		malloc( NodeCount	* sizeof(NODE)		);
	DESTree->Leaves			= (LEAF*)		malloc( LeafCount	* sizeof(LEAF)		);

	// Copy Data Over
	memcpy( DESTree->PolygonDeleted	, PolygonDeleted, PolyCount		* sizeof(bool)		);
	memcpy( DESTree->Polygons		, Polygons		, PolyCount		* sizeof(BSPPOLYGON));
	memcpy( DESTree->Planes			, Planes		, PlaneCount	* sizeof(PLANE)		);
	memcpy( DESTree->Nodes			, Nodes			, NodeCount		* sizeof(NODE)		);
	memcpy( DESTree->Leaves			, Leaves		, LeafCount		* sizeof(LEAF)		);

	// Now we need to allocate the internal arrays.
	// and copy over the specific data for each type
	for (int i = 0; i < PolyCount; i++ ) {
		// Vertices
		DESTree->Polygons[i].Vertices = nullptr;
		DESTree->Polygons[i].Vertices = (D3DVERTEX*)malloc( Polygons[i].VertexCount * sizeof(D3DVERTEX));
		memcpy( DESTree->Polygons[i].Vertices, Polygons[i].Vertices, Polygons[i].VertexCount * sizeof(D3DVERTEX));
		// Indices
		DESTree->Polygons[i].Indices = nullptr;
		DESTree->Polygons[i].Indices = (uint16_t*)malloc( Polygons[i].IndexCount * sizeof(uint16_t));
		memcpy( DESTree->Polygons[i].Indices, Polygons[i].Indices, Polygons[i].IndexCount * sizeof(uint16_t));
	} // Next i

	return DESTree;
}