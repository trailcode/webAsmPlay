/***************************************************************************
 *   Copyright (C) 2003 by Matthew L Tang                                  *
 *   mltang@carbon.cudenver.edu                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _Q3BSPRENDER_H_
#define _Q3BSPRENDER_H_

#include <vector>
#include <tceGeom/Q3BSPMap.h>
#include <tceGeom/Q3BSPBezierPatch.h>
#include "CBitset.h"
//#include "CEntityParser.h"

#define FACE_TYPE_NORMAL                    0x00000000
#define FACE_TYPE_OPAQUE                    0x00000010
#define FACE_TYPE_ALPHA_HOLE_NO_BLENDING    0x00000020
#define FACE_TYPE_ALPHA_AND_BLENDING        0x00000030
#define FACE_TYPE_BLENDING_ONLY             0x00000040

// entity_state_t->renderfx flags
#define RF_MINLIGHT             1               // allways have some light (viewmodel)
#define RF_VIEWERMODEL          2               // don't draw through eyes, only mirrors
#define RF_WEAPONMODEL          4               // only draw through eyes
#define RF_FULLBRIGHT           8               // allways draw full intensity
#define RF_DEPTHHACK            16              // for view weapon Z crunching
#define RF_TRANSLUCENT          32
#define RF_FRAMELERP            64
#define RF_BEAM                 128
#define RF_CUSTOMSKIN           256             // skin is an index in image_precache
#define RF_GLOW                 512             // pulse lighting for bonus items
#define RF_SHELL_RED            1024
#define RF_SHELL_GREEN          2048
#define RF_SHELL_BLUE           4096

namespace tce
{
	class Camera;
	
	/**
	 * Renders BSP maps
	 * Matthew L Tang
	 **/
	class Q3BSPRender : private geom::Q3BSPBezierPatch
	{
	
	public:
	    Q3BSPRender ();
	    Q3BSPRender (const Q3BSPRender & render);
	    ~Q3BSPRender ();
	    Q3BSPRender & operator=(const Q3BSPRender & render);
	    void renderMap ();
	    void setupOpenGLEXTS();
	    bool setMap (geom::quake3::Q3BSPMap * map);
	    bool setCamera (Camera * cam);
	    void RenderFace (int faceIndex);
	    void RenderNormalFace(int faceIndex);
	    void RenderPatch (int faceIndex);
	    //doorsVector * doors;
	    float dTime;
	    std::vector < geom::quake3::Q3AnimMap * > v_animationMaps;
	
	    int findLeaf(const geom::Vec3f & pos) const;
	    
	    bool renderNormalFaces;
	    bool renderOpaqueFaces;
	    bool renderAlphaHoleNoBlendingFaces;
	    bool renderAlphaAndBlendingFaces;
	    bool renderBlendingOnlyFaces;
	    bool renderPatches;
	
	private:
	    void initTCMods();
	    void updateTCMods();
	    int IsClusterVisible (int current, int test);
	    bool AABBInFrustum(const geom::Vec3i &min, const geom::Vec3i &max);
	    void doTCMods(const geom::quake3::Q3BSPShaderLayer & layer);
	    void initFaceVectors();
	    int classifyFaceType(int faceIndex);
	    void updateAnimationMaps();
	
	    Camera *cam;
	    int camClusterIndex;
	    CBitset m_FacesDrawn;       // The bitset for the faces that have/haven't been drawn
	
	    // @@@ Move to a sorting class
	    // Temp variables for quick sort
	    int leftMark;
	    int rightMark;
	    int tmp;
	    void quickSort(std::vector<int> & array, int left, int right);
	
	    std::vector <int> v_normalFaces;
	    std::vector <int> v_opaqueFaces;
	    std::vector <int> v_alphaHoleNoBlendingFaces;
	    std::vector <int> v_alphaAndBlendingFaces;
	    std::vector <int> v_blendingOnlyFaces;
	
	    int lastTexture1;
	    int lastTexture2;
	
	    float currTime;
	
	    std::vector < geom::quake3::Q3BSPLayerTCMod * > tcMods;
	
	};
}
#endif
