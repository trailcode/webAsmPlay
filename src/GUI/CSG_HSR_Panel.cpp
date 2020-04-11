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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/geom/BSPTree.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
	//-----------------------------------------------------------------------------
	// Application specific Variables
	//-----------------------------------------------------------------------------
	int						rendering			= 1;				// Lets us know if the app is rendering OR
	int						quit				= 0;				// Is about to quit.

	BRUSH					*Brushes			= NULL;				// Brush Array
	int						BrushCount			= 0;				// Number Of Brushes.
	int						NumberOfTextures	= 3;				// Number Of Textures

	fvec3					vecLook				(  0,  0,    1);	// Camera Lookat Vector
	fvec3					vecUp				(  0,  1,    0);	// Camera Up Vector
	fvec3					vecRight			(  1,  0,    0);	// Camera Right Vector
	fvec3					vecPos				(  0, 10, -300);	// Camera Position Vector
	float					Pitch				= 0;				// Camera Pitch
	float					time_elapsed		= 0;				// time since previous frame

	int						g_iCSGBrush			= 0;				// Current Brush to perform the op with
	int						g_iCSGMode			= CSG_NONE;			// Current CSG Operation to be performed
	int						g_iLastTexture		= -1;				// Last texture used
	bool					g_bWireFrame		= false;			// Wireframe View
	bool					g_bSafeTransform	= true;				// Perform Safe Translation
	bool					g_bCompileFinal		= false;			// Compile and render Final instead of individual brushes
	bool					g_bDisplayHelp		= false;			// Display the help text
	bool					g_bHSR				= true;				// Perform HSR on Dynamic CSG Pass.

	CBSPTree				*FinalBSP			= NULL;				// Final BSP Tree

	int						LEFTKEY = 0, RIGHTKEY = 0, UPKEY = 0, DOWNKEY = 0, PGUPKEY = 0, PGDOWNKEY = 0;
	bool					RotateBrushes[3]	= {false, false, false};

	//-----------------------------------------------------------------------------
	// Function Prototypes
	//-----------------------------------------------------------------------------
	BRUSH				*LoadIWF(char *filename, uint16_t * MeshCount);
	void				InitPolygons(void);
	void				renderframe(void);
	void				UpdateViewpos(void);
	void				SetupFont(void);
	void				sleep( clock_t wait );
	void				LoadMWMFileIntoBrush(char *filename, BRUSH * Brush);
	void				UpdateBSPTrees( );
	BBOX				CalculateBoundingBox( BRUSH * brush , fmat4 * Matrix);
	bool				BoundingBoxesIntersect( BBOX BoxA, BBOX BoxB );
	void				SetupLights();
	void				LoadTextures(void);
	void				RotateBrush( long Brush, float X, float Y, float Z, fvec3 CentrePoint);
	void				CheckInput();
	void				FreeBrushes( BRUSH * lpBrushes, uint16_t MeshCount);

	//-----------------------------------------------------------------------------
	// Name : InitPolygons ()
	// Desc : Creates and initialises all the polygons which will be used to 
	//        create and render the scene.
	//-----------------------------------------------------------------------------
	void InitPolygons(void)
	{
		uint16_t		  MeshCount;
		// Load Level Brushes
		Brushes		= LoadIWF("CSGDemo2.iwf", &MeshCount);
		BrushCount	= MeshCount;
		LoadTextures();
	
		// Set initial object matrices as identity
		for ( int i = 0; i < BrushCount; i++ ) {
			Brushes[i].Matrix = fmat4(1.0f);
		} // Next Brush

		UpdateBSPTrees();
	} // END FUNCTION

	//-----------------------------------------------------------------------------
	// Name : LoadIWF ()
	// Desc : Loads in our scene data from file
	//-----------------------------------------------------------------------------
	BRUSH *LoadIWF(char *filename, uint16_t * MeshCount )
	{
		BRUSH			*lpBrushes		= NULL;
		IWFFILEHEADER	ifh;
		uint16_t			chunkhdr;
		uint32_t			chunklen, f;
		bool			EndOfFile		= false;
		bool			EndOfMeshes		= false;
		int				CurrentMesh		= -1;
		BRUSH			*CurrentBrush	= NULL;
		POLYGON			*CurrentFace	= NULL;

		// Open the specified file
		FILE *stream = fopen(filename, "rb");
		if ( stream == NULL ) goto ReadFailure;

		// Read in the file header
		fread( &ifh, sizeof(IWFFILEHEADER), 1, stream );

		// If this is not a valid scene file then return false
		if ( ifh.Identifier	!= 'IIWF' || ifh.Version != 1.01f) goto ReadFailure;

		while( !EndOfFile ) {
			// Read Chunk Header and length
			fread( &chunkhdr, sizeof(uint16_t), 1,  stream );
			fread( &chunklen, sizeof(uint32_t), 1, stream );

			switch (chunkhdr) {
				case CHUNK_BEGINMESHES:																		// Meshes Begin Here
					fread( MeshCount, sizeof(uint16_t), 1, stream);											// Read number of meshes in file
					fseek( stream, *MeshCount * sizeof(uint32_t), SEEK_CUR);									// Skip Mesh file positions
					fseek( stream, 8, SEEK_CUR);															// Skip 8 bytes of dummy info
					// Allocate memory for brushes
					lpBrushes = new BRUSH[*MeshCount];
					memset( lpBrushes, 0, *MeshCount * sizeof(BRUSH) );
					// Read in each mesh from file
					while ( !EndOfMeshes ) {
						// Read Chunk Header and length
						fread( &chunkhdr, sizeof(uint16_t), 1,  stream );
						fread( &chunklen, sizeof(uint32_t), 1, stream );
						switch (chunkhdr) {
							case CHUNK_MESHDETAILS:																// Mesh Detail Chunk
								CurrentMesh++;
								CurrentBrush = &lpBrushes[CurrentMesh];											// Store brush for easy access
								fread( &CurrentBrush->Bounds   , sizeof(BBOX) , 1, stream );					// Read Mesh Bounds
								fread( &CurrentBrush->FaceCount, sizeof(uint32_t), 1, stream );					// Read Mesh Facecount
								fseek( stream, 8, SEEK_CUR );													// Skip 8 Bytes of reserved
								break;
							case CHUNK_POLYS:																	// Polygon Chunk
								// Allocate memory for brush faces
								CurrentBrush->Faces = (POLYGON*)malloc(CurrentBrush->FaceCount * sizeof(POLYGON));
								memset( CurrentBrush->Faces, 0, CurrentBrush->FaceCount * sizeof(POLYGON));
								// Loop round and read each polygon
								for ( f = 0; f < CurrentBrush->FaceCount; f++ ) {
									CurrentFace = &CurrentBrush->Faces[f];										// Store Face for easy access
									fread( &CurrentFace->VertexCount, sizeof(uint16_t), 1, stream );				// Get Vertex Count
									fread( &CurrentFace->IndexCount , sizeof(uint16_t), 1, stream );				// Get Vertex Count
									fread( &CurrentFace->Normal,		sizeof(fvec3), 1, stream );		// Read polygon normal
									fread( &CurrentFace->TextureIndex,	sizeof(uint16_t), 1, stream );				// Read Texture LUT Index
									fseek( stream, sizeof(uint32_t), SEEK_CUR );									// Skip Face Style Flags
									fseek( stream, 8, SEEK_CUR );												// Skip 8 Bytes of reserved
									CurrentFace->Vertices = new D3DVERTEX[CurrentFace->VertexCount];			// Allocate memory for VertexList
									CurrentFace->Indices  = new uint16_t[CurrentFace->IndexCount];					// Allocate memory for VertexList
									memset(CurrentFace->Vertices, 0, CurrentFace->VertexCount * sizeof(D3DVERTEX));
									memset(CurrentFace->Indices,  0, CurrentFace->IndexCount * sizeof(uint16_t));
								} // Next Poly
								break;
							case CHUNK_VERTEXCOORDS:															// Vertex Coord Chunk
								for ( f = 0; f < CurrentBrush->FaceCount; f++ ) {
									CurrentFace = &CurrentBrush->Faces[f];										// Store Face for easy access
									for ( int v = 0; v < CurrentFace->VertexCount; v++ ) {
										fread( &CurrentFace->Vertices[v].x, sizeof(float), 1, stream );			// Read X Coordinate
										fread( &CurrentFace->Vertices[v].y, sizeof(float), 1, stream );			// Read Y Coordinate
										fread( &CurrentFace->Vertices[v].z, sizeof(float), 1, stream );			// Read Z Coordinate
										// Copy Vertex Normal while we are here
										CurrentFace->Vertices[v].Normal = CurrentFace->Normal;
									} // Next Vertex
								} // Next Poly
								break;
							case CHUNK_VERTEXTEXCOORDS:		// Vertex Texture Coord Chunk
								for ( f = 0; f < CurrentBrush->FaceCount; f++ ) {
									CurrentFace = &CurrentBrush->Faces[f];										// Store Face for easy access
									for ( int v = 0; v < CurrentFace->VertexCount; v++ ) {
										fread( &CurrentFace->Vertices[v].tu, sizeof(float), 1, stream );		// Read U Texture Coord
										fread( &CurrentFace->Vertices[v].tv, sizeof(float), 1, stream );		// Read V Texture Coord
									} // Next Vertex
								} // Next Poly
								break;
							case CHUNK_INDICES:				// Face Vertex Indices Chunk
								for ( f = 0; f < CurrentBrush->FaceCount; f++ ) {
									fread( CurrentBrush->Faces[f].Indices, CurrentBrush->Faces[f].IndexCount * sizeof(uint16_t), 1, stream );
								} // Next Face
								break;
							case CHUNK_ENDMESHES:
								EndOfMeshes = true;
								break;
							default:																					// If this chunk is not recognised.. Skip it
								fseek( stream, chunklen, SEEK_CUR );
								break;
						} // End Switch
					} // End While Still Reading Meshes
					break;
				case CHUNK_EOF:																				// End Of File
					EndOfFile = true;
					break;
				default:																					// If this chunk is not recognised.. Skip it
					fseek( stream, chunklen, SEEK_CUR );
					break;
			} // End Switch
		} // End While

		fclose	(stream);	
		return lpBrushes;

	ReadFailure:
		// If we failed to read, we drop here
		if (stream) fclose( stream );
		if (lpBrushes) FreeBrushes(lpBrushes, *MeshCount);
		return NULL;
	}	

	//-----------------------------------------------------------------------------
	// Name : LoadTextures () 
	// Desc : Loads the 3 textures for the level
	//-----------------------------------------------------------------------------
	void LoadTextures(void)  
	{
		//load in textures
		NumberOfTextures = 3;
		/*
		D3DXCreateTextureFromFile(lpDevice,"Textures\\checkered_floor1.jpg",	&lpTextureSurface[0]); 
		D3DXCreateTextureFromFile(lpDevice,"Textures\\metalrustyfloor1.jpg",	&lpTextureSurface[1]); 
		D3DXCreateTextureFromFile(lpDevice,"Textures\\curvyfloor.jpg",			&lpTextureSurface[2]); 
		*/
 
	} // END FUNCTION

	//-----------------------------------------------------------------------------
	// Name : UpdateBSPTrees ()
	// Desc : Compiles the Per Brush BSP Trees and performs the CSG / HSR
	//-----------------------------------------------------------------------------
	void UpdateBSPTrees( )
	{
		int		  f, b;
		bool	  ClipSolid1 = false, ClipSolid2 = false;
		CBSPTree *CSGTree = NULL, *TGTTree = NULL, *CPYTree = NULL;

		// Compile Mini-BSP Trees.
		for ( b = 0; b < BrushCount; b++ ){
			// Clear previously created tree
			if ( Brushes[b].BSPTree ) {
				delete ((CBSPTree*)Brushes[b].BSPTree);
				Brushes[b].BSPTree = NULL;
			} // End If
			if ( Brushes[b].BSPTree == NULL ) Brushes[b].BSPTree = new CBSPTree;
			((CBSPTree*)Brushes[b].BSPTree)->InitPolygons( &Brushes[b], 1, false );
		} // Next Brush

		// Setup our solid clipping flags
		switch ( g_iCSGMode ) {
			case CSG_DIFFERENCE:
				ClipSolid1 = true; ClipSolid2 = false; 
				break;
			case CSG_INTERSECTION:
				ClipSolid1 = false; ClipSolid2 = false; 
				break;
		} // End Switch


		if ( g_iCSGMode != CSG_NONE ) {
			// Store CSG Brush for easy access
			CSGTree = ((CBSPTree*)Brushes[g_iCSGBrush].BSPTree);
			// Perform op with CSG brush on each normal brush
			for ( b = 0; b < BrushCount; b++ ) {
				if ( b == g_iCSGBrush ) continue;
				if ( !BoundingBoxesIntersect(Brushes[g_iCSGBrush].Bounds, Brushes[b].Bounds) ) continue;
				// Store target tree for easy access
				TGTTree = ((CBSPTree*)Brushes[b].BSPTree);
				// Clip the tree against the CSG Tree
				CSGTree->ClipTree( CSGTree->RootNode, NULL, 0, TGTTree, ClipSolid1, true );
				// First Make a copy of the CSG brush and manipulate this, instead
				// of the actual CSG brush, remember that we may need to add polys to several
				// intersecting brushes.
				CPYTree = CSGTree->CopyTree();
				TGTTree->ClipTree( TGTTree->RootNode, NULL, 0, CPYTree, ClipSolid2, false);
				// When in Difference Mode Invert The Copied Trees polygons
				if ( g_iCSGMode == CSG_DIFFERENCE ) CPYTree->InvertPolys();
				// Add all surviving polygons to the target tree
				for ( f = 0; f < CPYTree->PolyCount; f++ ){
					if ( !CPYTree->PolygonDeleted[f] ) TGTTree->AddPolygonEX( &CPYTree->Polygons[f] );
				} // Next f
				// Free Up the Copy of the CSG Tree
				delete CPYTree; CPYTree = NULL;
				if ( g_iCSGMode == CSG_INTERSECTION) TGTTree->InitPolygons( &Brushes[b], 1, true);
			} // Next Brush
		} // End if CSG Mode != CSG_NONE

		// Now perform a union on all the resulting Trees.
		// THIS is how we perform pre-compile Hidden Surface removal.
		for ( b = 0; b < BrushCount; b++ ) {
			if ( b == g_iCSGBrush && g_iCSGMode != CSG_NONE) continue;
			((CBSPTree*)Brushes[b].BSPTree)->m_bMisc = false;
			for ( int o = 0; o < BrushCount; o++ ) {
				if ( o == b || (o == g_iCSGBrush && g_iCSGMode != CSG_NONE)) continue;
				if (((CBSPTree*)Brushes[o].BSPTree)->m_bMisc == true) continue;
				if ( BoundingBoxesIntersect(Brushes[o].Bounds, Brushes[b].Bounds) == true ) {
					((CBSPTree*)Brushes[o].BSPTree)->ClipTree( ((CBSPTree*)Brushes[o].BSPTree)->RootNode, NULL, 0, ((CBSPTree*)Brushes[b].BSPTree), true, true);
					((CBSPTree*)Brushes[b].BSPTree)->ClipTree( ((CBSPTree*)Brushes[b].BSPTree)->RootNode, NULL, 0, ((CBSPTree*)Brushes[o].BSPTree), true, false);
				} //End If Bound Boxes Intersect
				// Because we do a role reversal for each brush in the tree
				// the below line ensures that each tree is only clipped once.
				((CBSPTree*)Brushes[b].BSPTree)->m_bMisc = true;
			} // End For o
		} // End For b

		// Compile final BSP if enabled
		if (g_bCompileFinal ) {	
			// Delete the CSG Brush as we must not include
			// this brush in the final BSP Compile process.
			if ( g_iCSGMode != CSG_NONE ) {
				delete (CBSPTree*)Brushes[g_iCSGBrush].BSPTree;
				Brushes[g_iCSGBrush].BSPTree = NULL;
			} // End If
			if ( FinalBSP == NULL ) FinalBSP = new CBSPTree;
			// Do the final compile
			FinalBSP->InitPolygons( Brushes, BrushCount, true);
		} // End If
	}


	//-----------------------------------------------------------------------------
	// Name : CalculateBoundingBox ()
	// Desc : Calculates the bounds from the brush passed
	//-----------------------------------------------------------------------------
	BBOX CalculateBoundingBox( BRUSH * brush , fmat4 * Matrix)
	{
		BBOX tBBox;
		fvec3 vec;

		tBBox.Min = fvec3( 1000000.0f,  1000000.0f,  1000000.0f );
		tBBox.Max = fvec3(-1000000.0f, -1000000.0f, -1000000.0f );

		for ( size_t f = 0; f < brush->FaceCount; f++ ){
			for ( int v = 0; v < brush->Faces[f].VertexCount; v++ ){
			
				if ( Matrix == NULL ) {
					vec = *(fvec3*)&brush->Faces[f].Vertices[v];
				} else {
					vec = *Matrix * fvec4(brush->Faces[f].Vertices[v].x, brush->Faces[f].Vertices[v].y, brush->Faces[f].Vertices[v].z, 1.0f);
				} // End If Safe Transform

				if ( vec.x < tBBox.Min.x ) tBBox.Min.x = vec.x;
				if ( vec.y < tBBox.Min.y ) tBBox.Min.y = vec.y;
				if ( vec.z < tBBox.Min.z ) tBBox.Min.z = vec.z;

				if ( vec.x > tBBox.Max.x ) tBBox.Max.x = vec.x;
				if ( vec.y > tBBox.Max.y ) tBBox.Max.y = vec.y;
				if ( vec.z > tBBox.Max.z ) tBBox.Max.z = vec.z;

			} // End For v
		} // End For f

		return tBBox;
	}
	//-----------------------------------------------------------------------------
	// Name: BoundingBoxesIntersect()
	// Desc: Returns true if the bounding boxes passed intersect.
	//-----------------------------------------------------------------------------
	bool BoundingBoxesIntersect( BBOX BoxA, BBOX BoxB )
	{
		dmessError("Implement me!");
		/*
		RECT Rect1, Rect2, DestRect;
    
		//First Do X/Z of bounding box
		Rect1.left = (long)BoxA.Min.x - 1; Rect1.right  = (long)BoxA.Max.x + 1;
		Rect1.top  = (long)BoxA.Min.z - 1; Rect1.bottom = (long)BoxA.Max.z + 1;
		Rect2.left = (long)BoxB.Min.x - 1; Rect2.right  = (long)BoxB.Max.x + 1;
		Rect2.top  = (long)BoxB.Min.z - 1; Rect2.bottom = (long)BoxB.Max.z + 1;
    
		if ( IntersectRect(&DestRect, &Rect1, &Rect2) == 0) return false;
    
		//Now Do X/Y of bounding box
		Rect1.left = (long)BoxA.Min.x - 1; Rect1.right  = (long)BoxA.Max.x + 1;
		Rect1.top  = (long)BoxA.Min.y - 1; Rect1.bottom = (long)BoxA.Max.y + 1;
		Rect2.left = (long)BoxB.Min.x - 1; Rect2.right  = (long)BoxB.Max.x + 1;
		Rect2.top  = (long)BoxB.Min.y - 1; Rect2.bottom = (long)BoxB.Max.y + 1;
    
		if ( IntersectRect(&DestRect, &Rect1, &Rect2) == 0) return false;
		*/
    
		return true;
	}

	//-----------------------------------------------------------------------------
	// Name : RotateBrush ()
	// Desc : Rotates a brush =)
	//-----------------------------------------------------------------------------
	void RotateBrush( long Brush, float X, float Y, float Z, fvec3 CentrePoint)
	{
		dmessError("Fix!");

		/*
		fmat4 rotmtx(1.0f);
		fquat XRot, YRot, ZRot;
	
		if( X ) D3DXQuaternionRotationAxis( &XRot, &fvec3( 1.0f, 0.0f, 0.0f ), X * g_DEGTORAD );
		if( Y ) D3DXQuaternionRotationAxis( &YRot, &fvec3( 0.0f, 1.0f, 0.0f ), Y * g_DEGTORAD );
		if( Z ) D3DXQuaternionRotationAxis( &ZRot, &fvec3( 0.0f, 0.0f, 1.0f ), Z * g_DEGTORAD );
		if( X ) fmat4Transformation( &rotmtx, NULL, NULL, NULL, &CentrePoint, &XRot , NULL );
		if( Y ) fmat4Transformation( &rotmtx, NULL, NULL, NULL, &CentrePoint, &YRot , NULL );
		if( Z ) fmat4Transformation( &rotmtx, NULL, NULL, NULL, &CentrePoint, &ZRot , NULL );

		if (!g_bSafeTransform ) {
			for ( size_t f = 0; f < Brushes[Brush].FaceCount; f++ ){
				for ( int v = 0; v < Brushes[Brush].Faces[f].VertexCount; v++ ){
					D3DXVec3TransformCoord( (fvec3*)&Brushes[Brush].Faces[f].Vertices[v], (fvec3*)&Brushes[Brush].Faces[f].Vertices[v], &rotmtx );
				} // End For v
			} // End For f
		} else {
			fmat4Multiply( &Brushes[Brush].Matrix, &Brushes[Brush].Matrix, &rotmtx);
		} // End If Safe Transform Test

		// Calculate Brushes Bounding Box
		Brushes[Brush].Bounds = CalculateBoundingBox( &Brushes[Brush], (g_bSafeTransform) ? &Brushes[Brush].Matrix : NULL);
		// We must first Calculate the Normal for ALL the faces
		// before the vertex normal calculations can be performed.
		for ( size_t f = 0; f < Brushes[Brush].FaceCount; f++ ){
			Brushes[Brush].Faces[f].Normal = CalculatePolyNormal( Brushes[Brush].Faces[f].Vertices[Brushes[Brush].Faces[f].Indices[0]], 
																  Brushes[Brush].Faces[f].Vertices[Brushes[Brush].Faces[f].Indices[1]], 
																  Brushes[Brush].Faces[f].Vertices[Brushes[Brush].Faces[f].Indices[Brushes[Brush].Faces[f].IndexCount - 1]], 
																  (g_bSafeTransform) ? &Brushes[Brush].Matrix : NULL );
			for ( int v = 0; v < Brushes[Brush].Faces[f].VertexCount; v++ ) {
				Brushes[Brush].Faces[f].Vertices[v].Normal = Brushes[Brush].Faces[f].Normal;
			}
		} // End For Face Count
		*/
	}

	//-----------------------------------------------------------------------------
	// Name : DoInit ()
	// Desc : Main Initialisation function, sets up main window, and calls
	//		  other needed initialisation functions.
	//-----------------------------------------------------------------------------
	static void doInit()
	{	
		static bool didInit = false;

		if(didInit) { return ;}

		didInit = true;

		InitPolygons();
	} // END FUNCTION

	//-----------------------------------------------------------------------------
	// Name : FreeBrushes ()
	// Desc : Frees up any brushes created
	//-----------------------------------------------------------------------------
	void FreeBrushes( BRUSH * lpBrushes, uint16_t MeshCount)
	{
		if ( lpBrushes ) {
			for (int b = 0; b < MeshCount; b++ ) {
				if (lpBrushes[b].Faces) {
					for (size_t f = 0; f < lpBrushes[b].FaceCount; f++ ) {
						delete []lpBrushes[b].Faces[f].Vertices;
						delete []lpBrushes[b].Faces[f].Indices;
					} // Next Face
					free(lpBrushes[b].Faces);
				} // End If
				if (lpBrushes[b].BSPTree) delete ((CBSPTree*)lpBrushes[b].BSPTree);
			}// Next Brush
			delete []lpBrushes;
		}// End if Brushes
	}
}

//-----------------------------------------------------------------------------
// Name : CalculatePolyNormal ()
// Desc : When passed three vertices, returns the normal.
//-----------------------------------------------------------------------------
fvec3 CalculatePolyNormal( D3DVERTEX v1, D3DVERTEX v2, D3DVERTEX v3, fmat4 *Matrix )
{
	fvec3 Normal;
	fvec3 vec0, vec1, vec2;

	if ( Matrix == NULL ) {
		vec0 = *(fvec3*)&v1;
		vec1 = *(fvec3*)&v2;
		vec2 = *(fvec3*)&v3;
	} else {
		vec0 = *Matrix * fvec4(v1.x, v1.y, v1.z, 1.0f);
		vec1 = *Matrix * fvec4(v2.x, v2.y, v2.z, 1.0f);
		vec2 = *Matrix * fvec4(v3.x, v3.y, v3.z, 1.0f);

	} // End If Matrix passed is NULL

	Normal = normalize(cross(vec1 - vec0, vec2 - vec0));
	
	return Normal;
}

//-----------------------------------------------------------------------------
// Name : EpsilonCompareVector ()
// Desc : Compares two vectors to see if they are equal within a tolerance.
//-----------------------------------------------------------------------------
bool EpsilonCompareVector( fvec3 Vec1, fvec3 Vec2 ) 
{
	if ( ( fabsf(Vec1.x - Vec2.x ) <= g_EPSILON) &&
			( fabsf(Vec1.y - Vec2.y ) <= g_EPSILON) &&
			( fabsf(Vec1.z - Vec2.z ) <= g_EPSILON)  ) return true;

	return false;
}

void GUI::CSG_HSR_Panel()
{
	if(!s_showCSG_HSR_Panel) { return ;}

	doInit();

	ImGui::Begin("CSG HSR Demo", &s_showCSG_HSR_Panel);

		const ImVec2 pos = ImGui::GetCursorScreenPos();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();

		const auto startPos = ImVec2(vMin.x + pos.x, vMin.y + pos.y);

		//dmess("Posa " << startPos.x << "," << startPos.y);

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		//s_KD_TreeTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));
		s_CSG_HSR_Canvas->setFrameBufferSize(__(sceneWindowSize), __(startPos) - __(pos));

        s_CSG_HSR_Canvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

	ImGui::End();
}

