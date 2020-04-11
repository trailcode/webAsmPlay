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

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const float g_PI       =  3.14159265358979323846f; // Pi
const float g_DEGTORAD =  0.01745329251994329547f; // Degrees to Radians
const float g_EPSILON  =  0.001f;				   // Float Tolerance

typedef struct {								// IWF File Header
	uint32_t	Identifier;						// File identifier (should always be 'IIWF' )
	float		Version;						// File Version Number
	uint32_t	dwDummy;						// Dummy uint32_t
}IWFFILEHEADER;

struct D3DVERTEX							// Our Pre-Lit Multi-TexCoord Vertex Structure
{
    float		x, y, z;					// Vertex Position
	glm::fvec3	Normal;						// Vertex Normal
	float		tu , tv;					// Base Texture Coordinates	
};


struct POLYGON								// Actual Polygon storage structure
{
	D3DVERTEX	*Vertices;					// Pointer to List of vertices.
	uint16_t		*Indices;					// Actual index buffer
	uint16_t		VertexCount;				// Number of vertices stored
	uint16_t		IndexCount;					// Number of indices stored
	glm::fvec3 Normal;						// Polygons Normal Vector
	int			TextureIndex;				// Index into texture array
};

struct BBOX {								// Bounding Box Structure
	glm::fvec3 Min;						// Bounds Min Vector
	glm::fvec3 Max;						// Bounds Max Vector
};

struct BRUSH								// Stores the brush data
{
	BBOX		Bounds;						// Brushes Bounding Box
	POLYGON		*Faces;						// Polygon Data
	uint32_t	FaceCount;					// Face Count
	glm::mat4	Matrix;						// Brushes Translation Matrix
	void		*BSPTree;					// Brushes BSP Tree (each brush has one)
};

//-----------------------------------------------------------------------------
// Shared Functions
//-----------------------------------------------------------------------------
glm::fvec3 CalculatePolyNormal( D3DVERTEX v1, D3DVERTEX v2, D3DVERTEX v3, glm::mat4 *Matrix );
bool		EpsilonCompareVector( glm::fvec3 Vec1, glm::fvec3 Vec2 ) ;