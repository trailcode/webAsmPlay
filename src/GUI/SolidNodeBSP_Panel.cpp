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

#include <glm/glm.hpp>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
	struct Vertex
	{
		Vertex() {}

		Vertex(float posx, float posy, float posz, fvec4 color, fvec4 specular, float tu, float tv) : m_pos(posx, posy, posz), 
			m_color(color),
			m_specular(specular),
			m_UV(tu, tv)
		{
		}

		fvec3 m_pos;
		fvec4 m_color;
		fvec4 m_specular;
		fvec2 m_UV;
	};

	// TODO align by size
	struct Polygon
	{
		Vertex	  m_vertexList[10];		// Actual Vertex Data
		fvec3	  m_normal;				// Polygon Normal
		size_t	  m_numberOfVertices;	// Number of Vertices
		size_t	  m_numberOfIndices;	// Number of Indices
		size_t	  m_indices[30];		// Actual Index Data
		Polygon	* m_next;				// Linked List Next Poly
	};

	struct Node							// Node structure
	{
		Polygon	* m_splitter;			// Splitter poly for this node
		Node	* m_front;				// Front Node Pointer
		Node	* m_back;				// Back Node Pointer
		bool	  m_isLeaf;				// Is this a leaf node
		bool	  m_isSolid;			// Is this leaf node solid.
	};

	//-----------------------------------------------------------------------------
	// Variable Definitions & Constants
	//-----------------------------------------------------------------------------
	const int			CP_FRONT	= 1001;			// In Front of Plane
	const int			CP_BACK		= 1002;			// Behind Plane
	const int			CP_ONPLANE	= 1003;			// On Plane
	const int			CP_SPANNING	= 1004;			// Spanning Plane
	
	float				yaw		= 0.0f;				// Camera Yaw since last frame

	fvec3				look_vector  ( 0, 0, 1);	// Camera Look At Vector
	fvec3				up_vector    ( 0, 1, 0);	// Camera Up Vector
	fvec3				right_vector ( 1, 0, 0);	// Camera Right Vector
	fvec3				position     ( 0, 1, 5);	// Camera Position
						  
	Polygon				* polygonList	= nullptr;	// Top of Polygon Linked List
	Node				* rootNode		= nullptr;	// BSP Tree Root Node

	// The following is the data for the map itself.
	uint8_t BSPMAP []={	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
						0,0,2,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
						0,2,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,
						1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,
						0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,
						0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
						0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,3,1,
						0,2,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,
						0,1,0,0,0,0,1,2,0,0,0,1,0,0,0,1,0,0,0,1,
						0,1,0,0,0,1,2,0,0,0,0,1,1,0,0,0,0,0,0,1,
						0,1,0,0,0,1,0,0,0,0,0,3,1,0,0,0,0,0,0,1,
						0,1,0,1,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
						1,2,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,  /* 1=cube 2=/ 3=\	  */
						1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,1,1,1,1,0,1,0,0,0,0,0,1,1,1,1,1,1,1,1,
						0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,
						0,0,2,0,0,0,1,0,0,1,0,3,0,0,0,0,0,0,0,0,
						0,2,0,0,0,0,1,0,0,1,0,0,3,0,0,0,0,0,0,0,
						1,0,0,0,0,0,1,0,0,1,0,0,0,1,1,1,1,1,0,1,
						0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
						0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,
						0,1,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,3,1,
						0,2,0,0,0,0,0,1,0,1,1,1,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,1,0,0,3,1,0,0,0,1,0,0,0,1,
						0,1,0,0,0,0,1,2,0,0,0,1,0,0,0,1,0,0,0,1,
						0,1,0,0,0,1,2,0,0,0,0,1,1,0,0,0,0,0,0,1,
						0,1,0,0,0,1,0,0,0,0,0,3,1,0,0,0,0,0,0,1,
						0,1,0,1,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
						1,2,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,  /* 1=cube 2=/ 3=\	  */
						1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
						1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

	//-----------------------------------------------------------------------------
	// Function Declarations
	//-----------------------------------------------------------------------------
	Polygon	* selectBestSplitter(Polygon * PolyList);
	int		  classifyPoly		(Polygon * plane,	Polygon * Poly);
	void	  splitPolygon		(Polygon * Poly,	Polygon * Plane,	Polygon * FrontSplit, Polygon *BackSplit);
	Polygon	* addPolygon		(Polygon * Parent,	Vertex	* Vertices, size_t NumberOfVerts);
	
	//-----------------------------------------------------------------------------
	// Name : buildBSPTree () (Recursive)
	// Desc : Performs the actual BSP Compile when fed a polygon linked list and
	//        a valid node (first time this is called this will be the parent of
	//        the polygon linked list, and an already created root node.
	//-----------------------------------------------------------------------------
	void buildBspTree(Node * currentNode, Polygon * polyList)
	{

		Polygon * polyTest		= nullptr;
		Polygon * frontList		= nullptr;
		Polygon * backList		= nullptr;
		Polygon * nextPolygon	= nullptr;
		Polygon * frontSplit	= nullptr;
		Polygon * backSplit		= nullptr;

		// Select the best splitter for this set of polygons
		currentNode->m_splitter = selectBestSplitter(polyList);
	
		if(!currentNode->m_splitter)
		{
			currentNode->m_splitter =selectBestSplitter(polyList);
		}

		polyTest = polyList;

		// Begin the loop until we reach the end of the linked list.
		while ( polyTest != NULL )
		{
			// Remember to store because polytest->Next could be altered
			nextPolygon = polyTest->m_next;

			if ( polyTest != currentNode->m_splitter )
			{
				switch ( classifyPoly( currentNode->m_splitter, polyTest ) )
				{
					case CP_FRONT:

						polyTest->m_next	= frontList;
						frontList			= polyTest;

						break;

					case CP_BACK:

						polyTest->m_next	= backList;
						backList			= polyTest;	

						break;

					case CP_SPANNING:

						// Allocate two new polys for this fragment
						frontSplit		= new Polygon;
						backSplit		= new Polygon;

						memset(frontSplit, 0, sizeof(Polygon));

						memset(backSplit,0, sizeof(Polygon));

						// Split the poly into two fragments
						splitPolygon( polyTest, currentNode->m_splitter, frontSplit, backSplit);

						// delete the original poly
						delete polyTest;

						// Reshuffle linked list
						frontSplit->m_next	= frontList;
						frontList			= frontSplit;
						backSplit->m_next	= backList;
						backList			= backSplit;

						break;

					default:

						dmessError("Invalid State!");
				} // End Switch
			} // End If Making sure we don't test ourselves

			// Move onto the next polygon
			polyTest = nextPolygon;
		} // End While Loop

		// If there is nothing left in the front list
		// then add an empty node here, otherwise
		// carry on building the tree
		if (!frontList)
		{
			Node * leafnode			= new Node;

			memset(leafnode, 0, sizeof(leafnode));

			leafnode->m_isLeaf		= true;
			leafnode->m_isSolid		= false;	
			leafnode->m_front		= nullptr;
			leafnode->m_back		= nullptr;
			leafnode->m_splitter	= nullptr;

			currentNode->m_front	= leafnode;

		}
		else
		{
			Node * newnode		= new Node;

			memset(newnode, 0, sizeof(newnode));

			newnode		->m_isLeaf	= false;
			currentNode	->m_front	= newnode;

			buildBspTree(newnode, frontList);

		} // End if frontlist is empty

		// If there is nothing left in the back list
		// then add a solid node here, otherwise
		// carry on building the tree
		if (!backList)
		{
			Node * leafnode		= new Node;

			memset(leafnode, 0, sizeof(leafnode));

			leafnode->m_isLeaf		= true;
			leafnode->m_isSolid		= true;
			leafnode->m_front		= nullptr;
			leafnode->m_back		= nullptr;
			leafnode->m_splitter	= nullptr;

			currentNode->m_back		= leafnode;
		}
		else
		{
			Node * newnode		= new Node;

			memset(newnode, 0, sizeof(newnode));

			newnode		->m_isLeaf	= false;
			currentNode	->m_back	= newnode;

			buildBspTree(newnode, backList);

		} // End if backlist is empty

	} // End Function

	//-----------------------------------------------------------------------------
	// Name : classifyPoly()
	// Desc : Classifies a polygon against the plane passed
	//-----------------------------------------------------------------------------
	int classifyPoly(Polygon * plane, Polygon * poly)
	{
		auto vec1 = &plane->m_vertexList[0];

		int			Infront = 0, Behind = 0, OnPlane = 0;
	
		// Loop round each of the vertices
		for ( int a = 0; a < poly->m_numberOfVertices; a++ )
		{
			const auto vec2			= &poly->m_vertexList[a];
			const auto direction	= vec1->m_pos - vec2->m_pos;
			const auto result		= dot(direction, plane->m_normal);

			// Tally up the position of each vertex
			if ( result > 0.001f )
			{
				Behind++;
			} else if (result < -0.001f) {
				Infront++;
			} else {
				OnPlane++;
				Infront++;
				Behind++;
			} // End if 
		} // Next Vertex

		if (OnPlane == poly->m_numberOfVertices) { return CP_FRONT ;}
		if (Behind  == poly->m_numberOfVertices) { return CP_BACK  ;}
		if (Infront == poly->m_numberOfVertices) { return CP_FRONT ;}

		return CP_SPANNING;
	}

	//-----------------------------------------------------------------------------
	// Name : classifyPoint()
	// Desc : Classifies a point against the plane passed
	//-----------------------------------------------------------------------------
	int classifyPoint( fvec3 * pos, Polygon * plane)
	{
	
		const auto vec1			= &plane->m_vertexList[0];
		const auto direction	= vec1->m_pos - (*pos);
		const auto result		= dot(direction, plane->m_normal);

		if (result < -0.001f) { return CP_FRONT ;}
		if (result >  0.001f) { return CP_BACK  ;}

		return CP_ONPLANE;
	}

	//-----------------------------------------------------------------------------
	// Name : walkBspTree() (Recursive)
	// Desc : Performs a BSP Rendering Traversal
	//-----------------------------------------------------------------------------
	void walkBspTree(Node * node, fvec3 * pos)
	{
		if  (node->m_isLeaf) { return ;}

		const auto result = classifyPoint(pos, node->m_splitter);
	
		if ( result == CP_FRONT )
		{
			if (node->m_back) { walkBspTree(node->m_back, pos) ;}

			/*
			lpDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, Node->Splitter->NumberOfVertices,
											 Node->Splitter->NumberOfIndices / 3, &Node->Splitter->Indices[0],
											 D3DFMT_INDEX16, &Node->Splitter->VertexList[0], sizeof(D3DLVERTEX));
											 */

			if (node->m_front) { walkBspTree(node->m_front, pos) ;}

			return;
		} // End if camera is in front

		// this happens if we are at back or on plane
		if (node->m_front!=NULL) { walkBspTree(node->m_front, pos) ;}
		if (node->m_back !=NULL) { walkBspTree(node->m_back,  pos) ;}

		return;
	}

	//-----------------------------------------------------------------------------
	// Name : initPolygons ()
	// Desc : Add's all the required poly's for the tree to start compilation
	//-----------------------------------------------------------------------------
	void initPolygons(void)
	{
		Vertex	  VERTLIST[4][4];
		Polygon	* child	= nullptr;
		int		  direction[4];

		polygonList	= nullptr;

		for (int y = 0; y < 40; y++)
		for (int x = 0; x < 20; x++)
		{
			memset(direction, 0, sizeof(int) * 4);
			
			int offset = (y * 20) + x;
						
			if (BSPMAP[offset] != 0) {
			
				if (BSPMAP[offset] == 2) {	
					VERTLIST[0][0] = { x-10.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
					VERTLIST[0][1] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
					VERTLIST[0][2] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
					VERTLIST[0][3] = { x-10.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
					direction[0]   = 1;
					
					if (x > 0) {
						if (BSPMAP[offset-1] == 0) {
							VERTLIST[1][0] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[1][1] = { x-10.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[1][2] = { x-10.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[1][3] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[1]   = 1;
						} // End if 0

					} // End if X > 0

					if (y > 0) {
						if (BSPMAP[offset-20] == 0) {
							VERTLIST[2][0] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[2][1] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[2][2] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[2][3] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[2]   = 1;;
						} // End if 0
					} // End if Y > 0
							
				} // Endif 2
				
				if (BSPMAP[offset] == 3) {	
					VERTLIST[0][0] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
					VERTLIST[0][1] = { x-9.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
					VERTLIST[0][2] = { x-9.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
					VERTLIST[0][3] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
					direction[0]   = 1;
	
					if (x < 19) {
						if (BSPMAP[offset+1] == 0) {
							VERTLIST[1][0] = { x-9.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[1][1] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[1][2] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[1][3] = { x-9.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[1]   = 1;
						} // Endif 0
					} // End if X < 19

					if (y > 0) {
						if (BSPMAP[offset-20] == 0) {
							VERTLIST[2][0] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[2][1] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[2][2] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[2][3] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[2]   = 1;
						} // End if 0
					} // End if Y > 0

				} // Endif 3				
				
				if (BSPMAP[offset] == 1) {
					if (x > 0) {
						if (BSPMAP[offset-1] == 0) {
							VERTLIST[0][0] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[0][1] = { x-10.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[0][2] = { x-10.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[0][3] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[0]   = 1;
						} // End if 0

					} // End if X > 0
 
					if (x < 19) {
						if (BSPMAP[offset+1] == 0) {
							VERTLIST[1][0] = { x-9.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[1][1] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[1][2] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[1][3] = { x-9.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[1]   = 1;
						} // Endif 0
					} // End if X < 19
	

					if (y > 0) {
						if (BSPMAP[offset-20] == 0) {
							VERTLIST[2][0] = { x-9.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[2][1] = { x-10.5f,3.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[2][2] = { x-10.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[2][3] = { x-9.5f,0.0f,(20.0f-y)+0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[2]   = 1;;
						} // End if 0
					} // End if Y > 0

					if(y < 39) {	
						if (BSPMAP[offset+20] == 0) {
							VERTLIST[3][0] = { x-10.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,0 };
							VERTLIST[3][1] = { x-9.5f,3.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,0 };
							VERTLIST[3][2] = { x-9.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},1,1 };
							VERTLIST[3][3] = { x-10.5f,0.0f,(20.0f-y)-0.5f,{ 1, 1, 1, 1}, {0,0,0,0},0,1 };
							direction[3]   = 1;;
						} // End if 0
					} // End if Y < 39
				}// end for if offset==1
	  
				for (int a = 0; a < 4; a++) {
					if (direction[a] != 0) {
						if (!polygonList)
						{
							polygonList = addPolygon(nullptr, &VERTLIST[a][0],4);
							child		= polygonList;
						} else {
							child		= addPolygon(child,&VERTLIST[a][0],4);
						} // End if PolygonList == NULL
					} // End if Direction != 0
				} // Next
			} // end for if offset!=0
		} // Next Y Component

		// ok, the polygons are in place, lets make like a tree=)
		rootNode = new Node;

		buildBspTree(rootNode, polygonList);
	}

	//-----------------------------------------------------------------------------
	// Name : getIntersect ()
	// Desc : Get the ray / plane intersection details
	//-----------------------------------------------------------------------------
	bool getIntersect (fvec3 *linestart, fvec3 *lineend, fvec3 *vertex, fvec3 *normal, fvec3 * intersection, float *percentage)
	{
		fvec3 direction  , L1;
		float linelength , dist_from_plane;

		direction.x = lineend->x - linestart->x;
		direction.y = lineend->y - linestart->y;
		direction.z = lineend->z - linestart->z;

		linelength  = dot(direction, *normal);
	
		if ( fabsf( linelength ) < 0.001f) return false; 

		L1.x = vertex->x - linestart->x;
		L1.y = vertex->y - linestart->y;
		L1.z = vertex->z - linestart->z;

		dist_from_plane = dot(L1, *normal);

		// How far from Linestart , intersection is as a percentage of 0 to 1 
		*percentage		= dist_from_plane / linelength; 

		// The ray does not reach, or is in front of the plane
		if (*percentage < 0.0f || *percentage > 1.0f) return false;

		// add the percentage of the line to line start
		intersection->x = linestart->x + direction.x * (*percentage);
		intersection->y = linestart->y + direction.y * (*percentage);
		intersection->z = linestart->z + direction.z * (*percentage);	

		return true;
	}

	//-----------------------------------------------------------------------------
	// Name : SplitPolygon ()
	// Desc : This function is used to do ALL the clipping and Splitting of 
	//		  polygons. It takes a polygon and a Plane and splits 
	//        the polygon into to two seperate polygons. When used for clipping to 
	//        a plane, call this function and then simply discard the front or
	//        back depending on your needs.
	// NOTE : FRONT and BACK MUST be valid pointers to empty Polygon structures as 
	//        this function does NOT allocate the memory for them. The reason for
	//        this is that this function is used in so many cases and some of them
	//        required the Front and Back already be initialized. 
	//-----------------------------------------------------------------------------
	void splitPolygon(Polygon *Poly, Polygon *Plane, Polygon *FrontSplit, Polygon *BackSplit)
	{
		// 50 is used here, as we should never
		// really have more points on a portal than this.
		Vertex		FrontList[50];
		Vertex		BackList[50];
		int				FrontCounter = 0;
		int				BackCounter = 0;

		int				PointLocation[50];

		int				InFront = 0, Behind = 0, OnPlane = 0;
		int				CurrentVertex = 0, Location = 0;

		// Determine each points location relative to the plane.
		for ( int i = 0; i < Poly->m_numberOfVertices; i++)
		{
			Location = classifyPoint(&Poly->m_vertexList[i].m_pos, Plane);

			if (Location == CP_FRONT )
				InFront++;
			else if (Location == CP_BACK )
				Behind++;
			else
				OnPlane++;

			PointLocation[i] = Location;
		} // Next Vertex
	
		// We set the VertexList[0] location again at the end
		// of the array so that we don't have to check and loop later
		//PointLocation[Poly->NumberOfVertices] = PointLocation[0];

		if (!InFront) {
			memcpy(BackList, Poly->m_vertexList, Poly->m_numberOfVertices * sizeof(Vertex));
			BackCounter = Poly->m_numberOfVertices;
		}

		if (!Behind) {
			memcpy(FrontList, Poly->m_vertexList, Poly->m_numberOfVertices * sizeof(Vertex));
			FrontCounter = Poly->m_numberOfVertices;
		}

		if (InFront && Behind) {
			for ( size_t i = 0; i < Poly->m_numberOfVertices; i++) {
				// Store Current vertex remembering to MOD with number of vertices.
				CurrentVertex = (i+1) % Poly->m_numberOfVertices;

				if (PointLocation[i] == CP_ONPLANE ) {
					FrontList[FrontCounter] = Poly->m_vertexList[i];
					FrontCounter++;
					BackList[BackCounter] = Poly->m_vertexList[i];
					BackCounter++;
					continue; // Skip to next vertex
				}
				if (PointLocation[i] == CP_FRONT ) {
					FrontList[FrontCounter] = Poly->m_vertexList[i];
					FrontCounter++;
				} else {
					BackList[BackCounter] = Poly->m_vertexList[i];
					BackCounter++;
				}
			
				// If the next vertex is not causing us to span the plane then continue
				//if (PointLocation[i+1] == CP_ONPLANE || PointLocation[i+1] == PointLocation[i]) continue;
				if (PointLocation[CurrentVertex] == CP_ONPLANE || PointLocation[CurrentVertex] == PointLocation[i]) continue;
			
				// Otherwise create the new vertex
				fvec3 IntersectPoint;
				float		percent;

				getIntersect( &Poly->m_vertexList[i].m_pos, &Poly->m_vertexList[CurrentVertex].m_pos, &Plane->m_vertexList[0].m_pos, &Plane->m_normal, &IntersectPoint, &percent );

				// create new vertex and calculate new texture coordinate
				Vertex copy;
				float deltax	= Poly->m_vertexList[CurrentVertex].m_UV.x - Poly->m_vertexList[i].m_UV.x;
				float deltay	= Poly->m_vertexList[CurrentVertex].m_UV.y - Poly->m_vertexList[i].m_UV.y;
				float texx		= Poly->m_vertexList[i].m_UV.x + ( deltax * percent );
				float texy		= Poly->m_vertexList[i].m_UV.y + ( deltay * percent );
				copy.m_pos		= IntersectPoint;
				copy.m_color	= Poly->m_vertexList[i].m_color;
				copy.m_UV		= { texx, texy };

				BackList[BackCounter++]		= copy;
				FrontList[FrontCounter++]	= copy;
			}
		}

		//OK THEN LETS BUILD THESE TWO POLYGONAL BAD BOYS

		FrontSplit->m_numberOfVertices = 0;
		BackSplit->m_numberOfVertices  = 0;

		// Copy over the vertices into the new polys
		FrontSplit->m_numberOfVertices = FrontCounter;
		memcpy(FrontSplit->m_vertexList, FrontList, FrontCounter * sizeof(Vertex));
		BackSplit->m_numberOfVertices  = BackCounter;
		memcpy(BackSplit->m_vertexList, BackList, BackCounter * sizeof(Vertex));


		BackSplit->m_numberOfVertices	= ( BackSplit->m_numberOfVertices  - 2 ) * 3;
		FrontSplit->m_numberOfVertices	= ( FrontSplit->m_numberOfVertices - 2 ) * 3;

		// Fill in the Indices
		short IndxBase;
		for ( short loop = 0, v1 = 1, v2 = 2; loop < FrontSplit->m_numberOfVertices/3; loop++, v1 = v2, v2++) {
			IndxBase = loop * 3;
			FrontSplit->m_indices[ IndxBase    ] =  0;
			FrontSplit->m_indices[ IndxBase + 1] = v1;
			FrontSplit->m_indices[ IndxBase + 2] = v2;
		} // Next Tri

		for ( size_t loop = 0, v1 = 1, v2 = 2; loop < BackSplit->m_numberOfVertices/3; loop++, v1 = v2, v2++) {
			IndxBase = loop * 3;
			BackSplit->m_indices[ IndxBase    ] =  0;
			BackSplit->m_indices[ IndxBase + 1] = v1;
			BackSplit->m_indices[ IndxBase + 2] = v2;
		} // Next Tri

		// Copy Extra Values
		FrontSplit->m_normal		= Poly->m_normal;
		BackSplit->m_normal		= Poly->m_normal;
	}

	//-----------------------------------------------------------------------------
	// Name : addPolygon ()
	// Desc : Takes any convex Polygon and breaks in into multiple Indexed Triangle 
	//        Lists and adds the polygon to a Linked list that will be sent to 
	//        the BSP Compiler.
	//-----------------------------------------------------------------------------
	Polygon * addPolygon( Polygon * parent, Vertex * vertices, size_t numVerts)
	{
		Polygon * child				= new Polygon;

		child->m_numberOfVertices	= numVerts;
		child->m_numberOfIndices	= (numVerts - 2) * 3;
		child->m_next				= nullptr;

		// Copy Vertices
		for ( size_t loop = 0; loop < numVerts; ++loop) { child->m_vertexList[loop] = vertices[loop] ;}

		// Calculate indices
		size_t v0,v1,v2;
		for ( size_t loop = 0; loop < child->m_numberOfIndices / 3; loop++ ){

			if ( loop == 0 ) { 
				v0 = 0; v1 = 1; v2 = 2;
			} else {
				v1 = v2; v2++;
			} // End if Loop == 0
	
			child->m_indices[ loop * 3     ] = v0;
			child->m_indices[(loop * 3) + 1] = v1;
			child->m_indices[(loop * 3) + 2] = v2;
		} // Next Tri

		// generate polygon normal
		const auto & vec0 = child->m_vertexList[0].m_pos;
		const auto & vec1 = child->m_vertexList[1].m_pos;
		const auto & vec2 = child->m_vertexList[child->m_numberOfVertices-1].m_pos;// the last vert


		const auto edge1 = vec1 - vec0;
		const auto edge2 = vec2 - vec0;

		child->m_normal = normalize(cross(edge1, edge2));

		if(parent) { parent->m_next=child ;}

		return child;
	}

	//-----------------------------------------------------------------------------
	// Name : selectBestSplitter ()
	// Desc : Selects the next best splitting plane from the poly linked list passed
	//----------------------------------------------------------------------------- 
	Polygon * selectBestSplitter( Polygon *PolyList )
	{
 
		Polygon		*Splitter		= PolyList;
		Polygon		*CurrentPoly	= NULL;
		Polygon		*SelectedPoly	= NULL; 
		uint64_t	 BestScore		= 100000;

		// Loop round all potential splitters
		while (Splitter!=NULL) {
			CurrentPoly = PolyList;
			int score = 0, splits = 0, backfaces = 0, frontfaces = 0;
		
			// Loop round all polys testing split counts etc
			while ( CurrentPoly != NULL ) {
				if ( CurrentPoly != Splitter ) {
					int result = classifyPoly(Splitter, CurrentPoly);
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
					} // End Switch
				} // End If splitter and current are not the same
				CurrentPoly = CurrentPoly->m_next;
			} // end while current poly
	 
			// Calculate Score
			score = abs(frontfaces-backfaces)+(splits*4);
			// Compare scores
			if ( score < BestScore ) {
				BestScore    = score;
				SelectedPoly = Splitter;
			} // End if Compare Score
  
			Splitter = Splitter->m_next;
		} // end while splitter == null	
		return SelectedPoly;
	}
}

void GUI::solidNodeBSP_Panel()
{
	if(!s_showSolidNodeBSP_Panel) { return ;}

	if(!rootNode) { initPolygons() ;}

	ImGui::Begin("Solid Node BSP Demo", &s_showSolidNodeBSP_Panel);

	ImGui::End();
}