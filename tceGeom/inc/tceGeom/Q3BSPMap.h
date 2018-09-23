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

#ifndef _Q3BSPMAP_H_
#define _Q3BSPMAP_H_

#include <string>
#include <vector>
#include <stack>

#include <tceGeom/plane.h>
#include <tceGeom/vec3.h>
#include <tceGeom/vec2.h>

#define Q3_BACK 0
#define Q3_FRONT 1

// @@@ The definations below have Q2 and Q3 definations mixed in them. Probable not correct
#define CONTENTS_SOLID 1    // an eye is never valid in a solid
#define CONTENTS_WINDOW 2   // translucent, but not watery
#define CONTENTS_AUX 4
#define CONTENTS_LAVA 8
#define CONTENTS_SLIME 16
#define CONTENTS_WATER 32
#define CONTENTS_MIST 64
#define LAST_VISIBLE_CONTENTS 64
// remaining contents are non-visible, and don't eat brushes
#define CONTENTS_AREAPORTAL 0x8000
#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000

// bot specific contents types
#define CONTENTS_MONSTERCLIP            0x20000
#define CONTENTS_TELEPORTER             0x40000
#define CONTENTS_JUMPPAD                0x80000
#define CONTENTS_CLUSTERPORTAL          0x100000
#define CONTENTS_DONOTENTER             0x200000
#define CONTENTS_BOTCLIP                0x400000

// currents can be added to any other contents, and may be mixed
#define CONTENTS_CURRENT_0 0x40000
#define CONTENTS_CURRENT_90 0x80000
#define CONTENTS_CURRENT_180 0x100000
#define CONTENTS_CURRENT_270 0x200000
#define CONTENTS_CURRENT_UP 0x400000
#define CONTENTS_CURRENT_DOWN 0x800000
#define CONTENTS_ORIGIN 0x1000000   // removed before bsping an entity
#define CONTENTS_MONSTER 0x2000000  // should never be on a brush, only in game
#define CONTENTS_DEADMONSTER 0x4000000
#define CONTENTS_DETAIL 0x8000000   // brushes to be added after vis leafs
#define CONTENTS_STRUCTURAL 0x10000000 // brushes used for the bsp
#define CONTENTS_TRANSLUCENT 0x10000000 // auto set if any surface has trans
#define CONTENTS_TRIGGER 0x40000000
#define CONTENTS_LADDER 0x20000000

#define SURF_LIGHT              0x1             // value will hold the light strength

#define SURF_SLICK              0x2             // effects game physics

#define SURF_SKY                0x4             // don't draw, but add to skybox
#define SURF_WARP               0x8             // turbulent water warp
#define SURF_TRANS33    0x10
#define SURF_TRANS66    0x20
#define SURF_FLOWING    0x40    // scroll towards angle
#define SURF_NODRAW             0x80    // don't bother referencing the texture


#define SURF_NODAMAGE 0x1 // never give falling damage
#define SURF_SLICK 0x2 // effects game physics
#define SURF_SKY 0x4 // lighting from environment map
#define SURF_LADDER 0x8
#define SURF_NOIMPACT 0x10 // don't make missile explosions
#define SURF_NOMARKS 0x20 // don't leave missile marks
#define SURF_FLESH 0x40 // make flesh sounds and effects
#define SURF_NODRAW 0x80 // don't generate a drawsurface at all
#define SURF_HINT 0x100 // make a primary bsp splitter
#define SURF_SKIP 0x200 // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHTMAP 0x400 // surface doesn't need a lightmap
#define SURF_POINTLIGHT 0x800 // generate lighting info at vertexes
#define SURF_METALSTEPS 0x1000 // clanking footsteps
#define SURF_NOSTEPS 0x2000 // no footstep sounds
#define SURF_NONSOLID 0x4000 // don't collide against curves with this set
#define SURF_LIGHTFILTER 0x8000 // act as a light filter during q3map -light
#define SURF_ALPHASHADOW 0x10000 // do per-pixel light shadow casting in q3map
#define SURF_NODLIGHT 0x20000 // don't dlight even if solid (solid lava, skies)

// This is the number that is associated with a face that is of type "polygon"
#define FACE_POLYGON    1
#define FACE_PATCH 2
#define FACE_MESH 3
#define FACE_BILLBOARD 4

#define ON_GROUND 0
#define IN_AIR 1
#define IN_WATER 2

class VertexBuffer; // @@@ TODO make an interface

namespace tce
{
	namespace geom
	{
		namespace quake3
		{
			struct Q3BSPVertex
			{
				Vec3f position;
				Vec2f textureCoord;
				Vec2f lightmapCoord;
				Vec3f normal;
				char color[4];
	
				Q3BSPVertex() : position(Vec3f()), textureCoord(Vec2f()), lightmapCoord(Vec2f()), normal(Vec3f()) { ;}
	
				void convertEndian()
				{
					position.convertEndian();
					textureCoord.convertEndian();
					lightmapCoord.convertEndian();
					normal.convertEndian();
				}
	
			};
	
			struct Q3BSPFace
			{
				int textureIndex;
				int effectIndex;
				int type;
				int startVertIndex;
				int numVerts;
				int meshVertIndex;
				int numMeshVerts;
				int lightmapIndex;
				
				// The face's lightmap corner in the image
				int lightmapCorner[2];
				
				// The size of the lightmap section
				int lightmapSize[2];
				
				// The 3D origin of lightmap.
				Vec3f lightmapPos;
				
				// The 3D space for s and t unit vectors.
				Vec3f lightmapVecs[2];
				
				// The face normal.
				Vec3f normal;
				
				// The bezier patch dimensions.
				int size[2];
	
				Q3BSPFace() : textureIndex(-1), effectIndex(-1), type(-1), startVertIndex(-1), numVerts(-1), meshVertIndex(-1), numMeshVerts(-1), lightmapIndex(-1), lightmapPos(Vec3f()), normal(Vec3f()) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(textureIndex);
					CONVERT_ENDIAN_UINT32(effectIndex);
					CONVERT_ENDIAN_UINT32(type);
					CONVERT_ENDIAN_UINT32(startVertIndex);
					CONVERT_ENDIAN_UINT32(numVerts);
					CONVERT_ENDIAN_UINT32(meshVertIndex);
					CONVERT_ENDIAN_UINT32(numMeshVerts);
					CONVERT_ENDIAN_UINT32(lightmapIndex);
					CONVERT_ENDIAN_UINT32(lightmapCorner[0]);
					CONVERT_ENDIAN_UINT32(lightmapCorner[1]);
					CONVERT_ENDIAN_UINT32(lightmapSize[0]);
					CONVERT_ENDIAN_UINT32(lightmapSize[1]);
					lightmapPos.convertEndian();
					lightmapVecs[0].convertEndian();
					lightmapVecs[1].convertEndian();
					normal.convertEndian();
					CONVERT_ENDIAN_UINT32(size[0]);
					CONVERT_ENDIAN_UINT32(size[1]);
				}
			};
	
			// The Vertexbuffer contains space for Tex1, Tex2, Vertex ( normal not supported yet )
			struct BSPPatch
			{
				int iSubPatches;
				VertexBuffer * pVB;
				int * iSizes;
			};
	
			struct Q3BSPLightmap
			{
				char imageBits[128][128][3];
			};
	
			struct Q3BSPNode
			{
				int planeIndex;
				int frontChildIndex;
				int backChildIndex;
				Vec3i boundingMin;
				Vec3i boundingMax;
	
				Q3BSPNode() : planeIndex(-1), frontChildIndex(-1), backChildIndex(-1), boundingMin(Vec3i()), boundingMax(Vec3i()) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(planeIndex);
					CONVERT_ENDIAN_UINT32(frontChildIndex);
					CONVERT_ENDIAN_UINT32(backChildIndex);
					boundingMin.convertEndian();
					boundingMax.convertEndian();
				}
	
			};
	
			struct Q3BSPLeaf
			{
				int visibilityClusterIndex;
				int areaPortalIndex;
				Vec3i boundingMin;
				Vec3i boundingMax;
				int leafFaceIndex;
				int numLeafFaces;
				int leafBrushIndex;
				int numLeafBrushes;
	
				Q3BSPLeaf() : visibilityClusterIndex(-1), areaPortalIndex(-1), boundingMin(Vec3i()), boundingMax(Vec3i()), leafFaceIndex(-1), numLeafFaces(-1), leafBrushIndex(-1), numLeafBrushes(-1) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(visibilityClusterIndex);
					CONVERT_ENDIAN_UINT32(areaPortalIndex);
					boundingMin.convertEndian();
					boundingMax.convertEndian();
					CONVERT_ENDIAN_UINT32(leafFaceIndex);
					CONVERT_ENDIAN_UINT32(numLeafFaces);
					CONVERT_ENDIAN_UINT32(leafBrushIndex);
					CONVERT_ENDIAN_UINT32(numLeafBrushes);
				}
			};
	
			struct Q3BSPVisibilityData
			{
				int numClusters;
				int bytesPerCluster;
				unsigned char *bitsets;
	
				Q3BSPVisibilityData() : numClusters(-1), bytesPerCluster(-1), bitsets(NULL) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(numClusters);
					CONVERT_ENDIAN_UINT32(bytesPerCluster);
				}
			};
	
			struct BSPCluster
			{
				std::vector<size_t> leafs;
	
				BSPCluster() : leafs(std::vector<size_t>()) { ;}
			};
	
			struct Q3BSPBrush
			{
				int brushSideIndex;
				int numBrushSides;
				int textureIndex;
	
				Q3BSPBrush() : brushSideIndex(-1), numBrushSides(-1),  textureIndex(-1) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(brushSideIndex);
					CONVERT_ENDIAN_UINT32(numBrushSides);
					CONVERT_ENDIAN_UINT32(textureIndex);
				}
			};
	
			struct Q3BSPBrushSide
			{
				int planeIndex;
				int textureIndex;
	
				Q3BSPBrushSide() : planeIndex(-1), textureIndex(-1) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(planeIndex);
					CONVERT_ENDIAN_UINT32(textureIndex);
				}
			};
	
			struct Q3BSPLoadTexture
			{
				char name[64];
				int flags;
				int contents;
	
				Q3BSPLoadTexture() : flags(-1), contents(-1) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(flags);
					CONVERT_ENDIAN_UINT32(contents);
				}
			};
	
			struct Q3BSPTexture
			{
				int shader;
				int flags;
				int contents;
	
				Q3BSPTexture() : shader(-1), flags(-1), contents(-1) { ;}
			};
	
			struct Q3BSPShaderRef
			{
				char name[64];
				int surfaceFlags;
				int contentFlags;
	
				Q3BSPShaderRef() : surfaceFlags(-1), contentFlags(-1) { ;}
	
				void convertEndian()
				{
					CONVERT_ENDIAN_UINT32(surfaceFlags);
					CONVERT_ENDIAN_UINT32(contentFlags);
				}
	
			};
	
			/**
			* Model 0 is the main map, others are doors, gates, buttons, etc.
			* The models lump describes rigid groups of world geometry. The first model
			* correponds to the base portion of the map while the remaining models correspond
			* movable portions of the map, such as the map's doors, platforms, and buttons.
			* Each model has a list of faces and list of brushes; these are especially
			* important for the movable parts of the map, which (unlike the base portion of
			* the map) do not have BSP trees associated with them. There are a total of
			* length / sizeof(models) records in the lump, where length is the size of the
			* lump itself, as specified in the lump directory.
			* @see cmodel_t
			*/
			struct Q3BSPModel
			{
				float boundingBox[6];
				int firstFaceIndex;
				int numFaces;
				int firstBrushIndex;
				int numBrushes;
	
				Q3BSPModel() : firstFaceIndex(-1), numFaces(-1), firstBrushIndex(-1), numBrushes(-1) { ;}
	
				void convertEndian()
				{
					for(int i = 0; i < 5; ++i)
					{
						unsigned int tmp = *(reinterpret_cast<unsigned int*>(&boundingBox[i]));
						CONVERT_ENDIAN_UINT32(tmp);
						boundingBox[i] = *(reinterpret_cast<float*>(&tmp));
					}
	
					CONVERT_ENDIAN_UINT32(firstFaceIndex);
					CONVERT_ENDIAN_UINT32(numFaces);
					CONVERT_ENDIAN_UINT32(firstBrushIndex);
					CONVERT_ENDIAN_UINT32(numBrushes);
				}
			};
			
	
	#define CULL_FUNC_FRONT 0
	#define CULL_FUNC_BACK  1
	#define CULL_FUNC_NONE  2
	
			template <typename shaderNameType, typename paramContainer, typename layerContainer>
			struct Shader
			{
				Shader(): shaderName(shaderNameType()), params(paramContainer()), layers(layerContainer()) {;};
				Shader(const shaderNameType & _shaderName) : shaderName(_shaderName), params(paramContainer()), layers(layerContainer()) { ;}
	
				shaderNameType shaderName;
				paramContainer params;
				layerContainer layers;
			};
	
	#define TCMOD_TYPE_ROTATE                    0
	#define TCMOD_TYPE_SCALE                     1
	#define TCMOD_TYPE_SCROLL                    2
	#define TCMOD_TYPE_STRETCH_SIN               3
	#define TCMOD_TYPE_STRETCH_TRIANGLE          4
	#define TCMOD_TYPE_STRETCH_SQUARE            5
	#define TCMOD_TYPE_STRETCH_SAWTOOTH          6
	#define TCMOD_TYPE_STRETCH_INVERSESAWTOOTH   7
	#define TCMOD_TYPE_TRANSFORM                 8
	#define TCMOD_TYPE_TURB                      9
	
			struct Q3BSPLayerTCMod
			{
	
				unsigned char tcModType;
				float * data;
	
				enum {
				degPerSec = 0,
				currRot = 1,
	
				sScale = 0,
				tScale = 1,
	
				sSpeed = 0,
				tSpeed = 1,
				sCurr = 2,
				tCurr = 3,
	
				base = 0,
				amplitude = 1,
				phase = 2,
				freq = 3,
	
				m00 = 0,
				m01 = 1,
				m11 = 2,
				t0 = 3,
				t1 = 4
				};
	
				Q3BSPLayerTCMod() : tcModType(1), data(NULL) { ;}
			};
	
	#define SHADER_DEFORM_VERTEXES_TYPE_WAVE_SIN                0
	#define SHADER_DEFORM_VERTEXES_TYPE_WAVE_TRIANGLE           1
	#define SHADER_DEFORM_VERTEXES_TYPE_WAVE_SQUARE             2
	#define SHADER_DEFORM_VERTEXES_TYPE_WAVE_SAWTOOTH           3
	#define SHADER_DEFORM_VERTEXES_TYPE_WAVE_INVERSE_SAWTOOTH   4
	#define SHADER_DEFORM_VERTEXES_TYPE_NORMAL_SIN              5
	#define SHADER_DEFORM_VERTEXES_TYPE_NORMAL_TRIANGLE         6
	#define SHADER_DEFORM_VERTEXES_TYPE_NORMAL_SQUARE           7
	#define SHADER_DEFORM_VERTEXES_TYPE_NORMAL_SAWTOOTH         8
	#define SHADER_DEFORM_VERTEXES_TYPE_NORMAL_INVERSE_SAWTOOTH 9
	#define SHADER_DEFORM_VERTEXES_TYPE_BULGE                   10
	
			struct Q3BSPShaderDeformVertexes
			{
				unsigned char deformType;
				float * data;
	
				enum {
	
				base = 0,
				amplitude = 1,
				freq = 2,
				div = 3,
				phase = 4,
	
				bulgeWidth = 0,
				bulgeHeight = 1,
				bulgeSpeed = 2
				};
	
				Q3BSPShaderDeformVertexes() : deformType(1), data(NULL) { ;}
			};
	
	#define BLEND_NO_BLENDING                     0x00000000
	
	#define BLEND_SOURCE_GL_ONE                   0x00000001
	#define BLEND_SOURCE_GL_ZERO                  0x00000002
	#define BLEND_SOURCE_GL_DST_COLOR             0x00000003
	#define BLEND_SOURCE_GL_ONE_MINUS_DST_COLOR   0x00000004
	#define BLEND_SOURCE_GL_SRC_ALPHA             0x00000005
	#define BLEND_SOURCE_GL_ONE_MINUS_SRC_ALPHA   0x00000006
	
	#define BLEND_DESTIN_GL_ONE                                          (0x00000001 << 4)
	#define BLEND_DESTIN_GL_ZERO                                        (0x00000002 << 4)
	#define BLEND_DESTIN_GL_SRC_COLOR                           (0x00000003 << 4)
	#define BLEND_DESTIN_GL_ONE_MINUS_SRC_COLOR   (0x00000004 << 4)
	#define BLEND_DESTIN_GL_SRC_ALPHA                            (0x00000005 << 4)
	#define BLEND_DESTIN_GL_ONE_MINUS_SRC_ALPHA    (0x00000006 << 4)
	#define BLEND_DESTIN_GL_ONE_MINUS_DST_ALPHA    (0x00000007 << 4)
	
	#define BLEND_SOURCE_MASK 0x0000000F
	#define BLEND_DESTIN_MASK 0x000000F0
	
	#define ALPHA_FUNC_NO_ALPHA                       0
	#define ALPHA_FUNC_GREATER_THAN_0                 1
	#define ALPHA_FUNC_LESS_THAN_128                  2
	#define ALPHA_FUNC_GREATER_THAN_OR_EQUAL_TO_128   3
	
	#define DEPTH_FUNC_LESS_EQUAL   0
	#define DEPTH_FUNC_EQUAL        1
	
			struct Q3BSPShaderLayer
			{
				unsigned int blendingParms;
				unsigned int texture;
				bool depthWrite;
				unsigned char alphaFunc;
				unsigned char depthFunc;
				Q3BSPLayerTCMod * tcMods;
				unsigned char numOfTcMods;
				bool hasAnimMap;
				Q3BSPShaderLayer() :
						blendingParms(0),
						texture(0),
						depthWrite(true),
						alphaFunc(ALPHA_FUNC_NO_ALPHA),
						depthFunc(DEPTH_FUNC_LESS_EQUAL),
						tcMods(NULL),
						numOfTcMods(0),
						hasAnimMap(false) { ;}
			};
	
			struct Q3AnimMap
			{
				unsigned char numOfFrames; // @@@ Suposivaly only 8, bit compress?
				unsigned int * frameTextureID;
				float frequency;
				unsigned char currFrame;
	
				Q3AnimMap() :
						numOfFrames(0),
						frameTextureID(NULL),
						frequency(0),
				currFrame(0) { ;}
			};
	
			struct Q3BSPShaderLayerArray
			{
				unsigned char numOfLayers;
				Q3BSPShaderLayer * layers;
	
				Q3BSPShaderLayerArray() : numOfLayers(0), layers(NULL) { ;}
			};
	
			typedef Shader <std::string, std::stack < std::string * >, std::stack < std::stack < std::string * > * > > Q3BSPShaderParse;
	
			typedef Shader <const char *, unsigned int, Q3BSPShaderLayerArray *> Q3BSPShader;
	
			struct Q3BSPLights
			{
				unsigned char ambient[3];     // This is the ambient color in RGB
				unsigned char directional[3]; // This is the directional color in RGB
				unsigned char direction[2];   // The direction of the light: [phi,theta]
	
				Q3BSPLights() {}}
			;
	
	#define NUM_OF_MODELS           map->numModels
	#define P_MODELS                map->models
	#define NUM_OF_VERTS            map->numVerts
	#define P_VERTS                 map->verts
	#define NUM_OF_FACES            map->numFaces
	#define P_FACES                 map->faces
	#define NUM_OF_TEXTURES         map->numTextures
	#define NUM_OF_LIGHTMAPS        map->numLightmaps
	#define TEXTURES                map->textureMap
	#define LIGHTMAPS               map->lightmapMap
	#define NUM_OF_MESH_VERTS       map->numMeshVerts
	#define P_MESH_VERTS            map->meshVerts
	#define NUM_OF_NODES            map->numNodes
	#define P_NODES                 map->nodes
	#define NUM_OF_LEAFS            map->numLeafs
	#define P_LEAFS                 map->leafs
	#define NUM_OF_LEAF_FACES       map->numLeafFaces
	#define P_LEAF_FACES            map->leafFaces
	#define NUM_OF_PLANES           map->numPlanes
	#define P_PLANES                map->planes
	#define P_VIS_DATA              map->visibilityData
	#define P_CLUSTERS              map->clusters
	#define NUM_OF_CLUSTERS         map->numClusters
	#define NUM_OF_LEAF_BRUSHES     map->numLeafBrushes
	#define P_LEAF_BRUSHES          map->leafBrushes
	#define NUM_OF_BRUSHES          map->numBrushes
	#define P_BRUSHES               map->brushes
	#define NUM_OF_BRUSH_SIDES      map->numBrushSides
	#define P_BRUSH_SIDES           map->brushSides
	#define P_TEXTURES              map->textures
	#define P_PATCHES               map->patches
	#define P_SHADERS               map->shaderArray
	#define NUM_OF_SHADERS          map->numShaders
	
	#define P_SHADER_REFS           map->shaderRefs
	#define NUM_OF_SHADER_REFS      map->numShaderRefs
	#define P_INDICES               map->indexArray
	
	#define P_LEAF_MODELS   map->leafModels
	#define P_LIGHT_VOLS    map->lightVolumes
	#define NUM_OF_LIGHT_VOLS map->numLightVols
	
			struct Q3BSPMap
			{
				int numVerts;
				int numFaces;
				int numTextures;
				int numLightmaps;
	
				int numNodes;
				int numLeafs;
				int numLeafFaces;
				int numPlanes;
				int numLeafBrushes;
				int numBrushes;
				int numBrushSides;
				int n_numOfTextures;
	
				int numMeshVerts;
	
				Q3BSPVertex * verts;
				Q3BSPFace * faces;
	
				int * meshVerts;
	
				BSPPatch ** patches;
	
				Q3BSPNode * nodes;
				Q3BSPLeaf * leafs;
				Plane3f * planes;
				int * leafFaces;
	
				int * leafBrushes;
				Q3BSPVisibilityData * visibilityData;
				BSPCluster * clusters;
				int numClusters;
	
				Q3BSPBrush * brushes;
				Q3BSPBrushSide * brushSides;
	
				Q3BSPTexture * textures;
	
				unsigned int * textureMap;
				unsigned int * lightmapMap;
	
				int numModels;
				Q3BSPModel * models;
	
				Q3BSPShader * shaderArray;
				int numShaders;
	
				Q3BSPShaderRef * shaderRefs;
				int numShaderRefs;
	
				int * indexArray;
	
				std::vector < Q3AnimMap * > animationMaps;
	
				std::vector < int > * leafModels;
	
				Q3BSPLights * lightVolumes;
				int numLightVols;
	
			};
		}
	}
}

#endif
