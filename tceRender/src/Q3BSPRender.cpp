/***************************************************************************
 *   Copyright (C) 2003 by Matthew L Tang                                  *
 *   mltang@carbon.cudenver.edu                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>
//#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <list>
//#include <SDL.h>
#include <tceRender/Q3BSPRender.h>
#include <tceRender/Camera.h>
#include <tceGeom/Q3BSPMap.h>
#include <tceRender/VertexBuffer.h>
#include <tceRender/CGLState.h>
#include <tceCore/Debug.h>
//#include "drawStuff.h"

using namespace std;
using namespace tce;
using namespace tce::geom;
using namespace tce::geom::quake3;

/*
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p = 0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p = 0;
*/

Q3BSPRender::Q3BSPRender (): Q3BSPBezierPatch ()
{
    //cam = NULL;
    map = NULL;

    lastTexture1 = -1;
    lastTexture2 = -1;

    renderNormalFaces = true;
    renderOpaqueFaces = true;
    renderAlphaHoleNoBlendingFaces = true;
    renderAlphaAndBlendingFaces = true;
    renderBlendingOnlyFaces = true;
    renderPatches = true;

}

Q3BSPRender::Q3BSPRender (const Q3BSPRender & render)
{
    // TODO
	assert(false);
}

Q3BSPRender::~Q3BSPRender ()
{}

Q3BSPRender &
Q3BSPRender::operator=(const Q3BSPRender & render)
{
    // TODO
    return *this;
}

// @@@ Don't need this GLUE should take care of it
void
Q3BSPRender::setupOpenGLEXTS()
{
    /*
    glLockArraysEXT_p = (PFNGLLOCKARRAYSEXTPROC)
                        SDL_GL_GetProcAddress("glLockArraysEXT");
    glUnlockArraysEXT_p = (PFNGLUNLOCKARRAYSEXTPROC)
                          SDL_GL_GetProcAddress("glUnlockArraysEXT");
                          */
}

bool Q3BSPRender::setMap (Q3BSPMap * map)
{
    this->map = map;
    if (map)
    {
    	/*
        if (cam)
        {
            if (!cam->setMap (map)) { return false ;}
        }
        */
        
        m_FacesDrawn.Resize (NUM_OF_FACES);
    }

    initFaceVectors();

    initTCMods();

    return (bool) map;
}

bool Q3BSPRender::setCamera (Camera * cam)
{
    this->cam = cam;
    if (map && cam)
    {
        //if (!cam->setMap (map)) { return false ;}

    }
    return (bool) cam;
}

inline int
Q3BSPRender::IsClusterVisible (int current, int test)
{

    // This function is used to test the "current" cluster against
    // the "test" cluster.  If the "test" cluster is seen from the
    // "current" cluster, we can then draw it's associated faces, assuming
    // they aren't frustum culled of course.  Each cluster has their own
    // bitset containing a bit for each other cluster.  For instance, if there
    // is 10 clusters in the whole level (a tiny level), then each cluster
    // would have a bitset of 10 bits that store a 1 (visible) or a 0 (not visible)
    // for each other cluster.  Bitsets are used because it's faster and saves
    // memory, instead of creating a huge array of booleans.  It seems that
    // people tend to call the bitsets "vectors", so keep that in mind too.

    // Make sure we have valid memory and that the current cluster is > 0.
    // If we don't have any memory or a negative cluster, return a visibility (1).
    if (!P_VIS_DATA->bitsets || current < 0)
        return 1;

    // Use binary math to get the 8 bit visibility set for the current cluster
    unsigned char visSet =
        P_VIS_DATA->bitsets[(current * P_VIS_DATA->bytesPerCluster) +
                             (test / 8)];

    // Now that we have our vector (bitset), do some bit shifting to find if
    // the "test" cluster is visible from the "current" cluster, according to the bitset.
    int result = visSet & (1 << ((test) & 7));

    // Return the result ( either 1 (visible) or 0 (not visible) )
    return (result);
}

// @@@ Also in CFrustum kinda
inline bool
Q3BSPRender::AABBInFrustum (const Vec3i & min, const Vec3i & max)
{
	
	
	return true;
	
	/*
    // Go through all of the corners of the box and check then again each plane
    // in the frustum.  If all of them are behind one of the planes, then it most
    // like is not in the frustum.
    for (int i = 0; i < 6; i++)
    {
        if (cam->m_Frustum[i][A] * min.x +
                cam->m_Frustum[i][B] * min.y +
                cam->m_Frustum[i][C] * min.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * max.x +
                cam->m_Frustum[i][B] * min.y +
                cam->m_Frustum[i][C] * min.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * min.x +
                cam->m_Frustum[i][B] * max.y +
                cam->m_Frustum[i][C] * min.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * max.x +
                cam->m_Frustum[i][B] * max.y +
                cam->m_Frustum[i][C] * min.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * min.x +
                cam->m_Frustum[i][B] * min.y +
                cam->m_Frustum[i][C] * max.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * max.x +
                cam->m_Frustum[i][B] * min.y +
                cam->m_Frustum[i][C] * max.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * min.x +
                cam->m_Frustum[i][B] * max.y +
                cam->m_Frustum[i][C] * max.z + cam->m_Frustum[i][D] > 0)
            continue;
        if (cam->m_Frustum[i][A] * max.x +
                cam->m_Frustum[i][B] * max.y +
                cam->m_Frustum[i][C] * max.z + cam->m_Frustum[i][D] > 0)
            continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
    */
}

void
Q3BSPRender::initTCMods()
{
    for(int i = 0; i < NUM_OF_SHADERS; ++i)
    {
        const Q3BSPShader & shader = P_SHADERS[i];

        for(int j = 0; j < shader.layers->numOfLayers; ++j)
        {
            const Q3BSPShaderLayer & layer = shader.layers->layers[j];

            for(int k = 0; k < layer.numOfTcMods; ++k)
            {
                Q3BSPLayerTCMod & tcMod = layer.tcMods[k];

                switch(tcMod.tcModType)
                {
                case TCMOD_TYPE_ROTATE:

                    tcMods.push_back(&tcMod);

                    break;
                case TCMOD_TYPE_SCALE:

                    break;
                case TCMOD_TYPE_SCROLL:

                    tcMods.push_back(&tcMod);

                    break;
                case TCMOD_TYPE_STRETCH_SIN:
                    break;
                case TCMOD_TYPE_STRETCH_TRIANGLE:
                    break;
                case TCMOD_TYPE_STRETCH_SQUARE:
                    break;
                case TCMOD_TYPE_STRETCH_SAWTOOTH:
                    break;
                case TCMOD_TYPE_STRETCH_INVERSESAWTOOTH:
                    break;
                case TCMOD_TYPE_TRANSFORM:
                    break;
                case TCMOD_TYPE_TURB:
                    break;
                }

            }

        }
    }

    cout << "tcMods.size() = " << tcMods.size() << endl;
}

void
Q3BSPRender::updateTCMods()
{

    vector < Q3BSPLayerTCMod * >::iterator i;

    for(i = tcMods.begin(); i != tcMods.end(); ++i)
    {
        Q3BSPLayerTCMod & tcMod = **i;

        float * currRot;
        switch(tcMod.tcModType)
        {
        case TCMOD_TYPE_ROTATE:

            currRot = &tcMod.data[Q3BSPLayerTCMod::currRot];

            (*currRot) += tcMod.data[Q3BSPLayerTCMod::degPerSec] * dTime;

            if((*currRot) < 0)
            {

                (*currRot) += 360.0;

            }
            else if((*currRot) > 360)
            {
                (*currRot) -= 360;
            }

            break;
        case TCMOD_TYPE_SCALE:

            break;
        case TCMOD_TYPE_SCROLL:

            tcMod.data[Q3BSPLayerTCMod::sCurr] += tcMod.data[Q3BSPLayerTCMod::sSpeed] * dTime;
            tcMod.data[Q3BSPLayerTCMod::tCurr] += tcMod.data[Q3BSPLayerTCMod::tSpeed] * dTime;

            if(tcMod.data[Q3BSPLayerTCMod::sCurr] > 2)
            {
                tcMod.data[Q3BSPLayerTCMod::sCurr] -= 2;

            }
            else if(tcMod.data[Q3BSPLayerTCMod::sCurr] < -2)
            {

                tcMod.data[Q3BSPLayerTCMod::sCurr] += 2;

            }

            if(tcMod.data[Q3BSPLayerTCMod::tCurr] > 2)
            {
                tcMod.data[Q3BSPLayerTCMod::tCurr] -= 2;

            }
            else if(tcMod.data[Q3BSPLayerTCMod::tCurr] < -2)
            {

                tcMod.data[Q3BSPLayerTCMod::tCurr] += 2;

            }

            break;
        case TCMOD_TYPE_STRETCH_SIN:
            break;
        case TCMOD_TYPE_STRETCH_TRIANGLE:
            break;
        case TCMOD_TYPE_STRETCH_SQUARE:
            break;
        case TCMOD_TYPE_STRETCH_SAWTOOTH:
            break;
        case TCMOD_TYPE_STRETCH_INVERSESAWTOOTH:
            break;
        case TCMOD_TYPE_TRANSFORM:
            break;
        case TCMOD_TYPE_TURB:
            break;
        }
    }
}


inline void
Q3BSPRender::doTCMods(const Q3BSPShaderLayer & layer)
{

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    for(int j = 0; j < layer.numOfTcMods; ++j)
    {
        Q3BSPLayerTCMod & tcMod = layer.tcMods[j];
        float * currRot;
        switch(tcMod.tcModType)
        {
        case TCMOD_TYPE_ROTATE:
            currRot = &tcMod.data[Q3BSPLayerTCMod::currRot];
            glTranslatef(0.5,0.5,0);
            glRotatef(*currRot, 0,0,1);
            glTranslatef(-0.5,-0.5,0);

            break;
        case TCMOD_TYPE_SCALE:
            // @@@ Test
            glScalef(tcMod.data[Q3BSPLayerTCMod::sScale], tcMod.data[Q3BSPLayerTCMod::tScale], 0);
            break;
        case TCMOD_TYPE_SCROLL:
            glTranslatef(tcMod.data[Q3BSPLayerTCMod::sCurr], tcMod.data[Q3BSPLayerTCMod::tCurr], 0);

            break;
        case TCMOD_TYPE_STRETCH_SIN:
            break;
        case TCMOD_TYPE_STRETCH_TRIANGLE:
            break;
        case TCMOD_TYPE_STRETCH_SQUARE:
            break;
        case TCMOD_TYPE_STRETCH_SAWTOOTH:
            break;
        case TCMOD_TYPE_STRETCH_INVERSESAWTOOTH:
            break;
        case TCMOD_TYPE_TRANSFORM:
            break;
        case TCMOD_TYPE_TURB:
            break;
        }
    }
}


void
Q3BSPRender::RenderNormalFace(int faceIndex)
{
    const Q3BSPFace & pFace = P_FACES[faceIndex];

    // @@@ Does this do any good?
    //glUnlockArraysEXT();

    glVertexPointer(3,GL_FLOAT,sizeof(Q3BSPVertex),&(P_VERTS[pFace.startVertIndex].position));

    glActiveTextureARB (GL_TEXTURE0_ARB);

    if(pFace.textureIndex != TEXTURES[pFace.textureIndex])
    {
        glEnable (GL_TEXTURE_2D);
        lastTexture1 = TEXTURES[pFace.textureIndex];
        glBindTexture (GL_TEXTURE_2D, lastTexture1);

    }

    glActiveTextureARB (GL_TEXTURE1_ARB);

    if(pFace.lightmapIndex != -1)
    {
        if(pFace.lightmapIndex != LIGHTMAPS[pFace.lightmapIndex])
        {
            glEnable (GL_TEXTURE_2D);
            lastTexture2 = LIGHTMAPS[pFace.lightmapIndex];
            glBindTexture (GL_TEXTURE_2D, lastTexture2);

        }

        // Assign the second texture pass to point to the light map texture coordinates
        glClientActiveTextureARB(GL_TEXTURE1_ARB);

        glTexCoordPointer(2,
                          GL_FLOAT,
						  sizeof(Q3BSPVertex),
						  &(P_VERTS[pFace.startVertIndex].lightmapCoord));

    }
    else { glDisable(GL_TEXTURE_2D) ;}

    glClientActiveTextureARB(GL_TEXTURE0_ARB);

    glTexCoordPointer(2,
                      GL_FLOAT,sizeof(Q3BSPVertex),
					  &(P_VERTS[pFace.startVertIndex].textureCoord));


    // @@@ Does this do any good?
    //glLockArraysEXT_p(0, pFace.numMeshVerts);

    glDrawElements(	GL_TRIANGLES,
					pFace.numMeshVerts,
					GL_UNSIGNED_INT,
					&P_INDICES[pFace.meshVertIndex]);


}

inline void
Q3BSPRender::RenderFace (int faceIndex)
{
	//dmess("NUM_OF_FACES " << NUM_OF_FACES << " faceIndex " << faceIndex);
	
	Q3BSPFace * faces = map->faces;
	int numFaces = map->numFaces;
	
    // Here we grab the face from the index passed in
    const Q3BSPFace & pFace = P_FACES[faceIndex];

    glVertexPointer(3,GL_FLOAT,sizeof(Q3BSPVertex),&(P_VERTS[pFace.startVertIndex].position));

    Q3BSPShader & shader = P_SHADERS[P_TEXTURES[pFace.textureIndex].shader];

    CGLState::setCullFunc(shader);

    //dmess("pFace.numMeshVerts " << pFace.numMeshVerts << "pFace " << pFace.startVertIndex);
    
    //glLockArraysEXT_p(0, pFace.numMeshVerts);

    //dmess("shader.layers->numOfLayers " << shader.layers->numOfLayers);
    
    for(int i = (int)shader.layers->numOfLayers - 1; i >= 0; --i)
    {

        const Q3BSPShaderLayer & layer = shader.layers->layers[i];

        CGLState::setAlphaFunc(layer);
        CGLState::setBlendFunc(layer);
        CGLState::setDepthFunc(layer);
        CGLState::setDepthWrite(layer);

        //dmess("layer.texture " << layer.texture);

        if(layer.texture)
        {

            glTexCoordPointer(2,
                              GL_FLOAT,sizeof(Q3BSPVertex),&(P_VERTS[pFace.startVertIndex].textureCoord));

            glActiveTextureARB (GL_TEXTURE0_ARB);
            glEnable (GL_TEXTURE_2D);

            if(!layer.hasAnimMap)
            {
                if(layer.texture != lastTexture1)
                {
                    glBindTexture (GL_TEXTURE_2D, layer.texture);
                    lastTexture1 = layer.texture;
                }

            }
            else
            {

                Q3AnimMap * animMap = (Q3AnimMap *)layer.texture;

                if(lastTexture1 != animMap->frameTextureID[animMap->currFrame])
                {

                    lastTexture1 = animMap->frameTextureID[animMap->currFrame];
                    glBindTexture (GL_TEXTURE_2D, lastTexture1);


                }

            }

            if(layer.numOfTcMods)
            {

                doTCMods(layer);


                glDrawElements(GL_TRIANGLES, pFace.numMeshVerts,
                               GL_UNSIGNED_INT,&P_INDICES[pFace.meshVertIndex]);


                glPopMatrix();

                glMatrixMode(GL_MODELVIEW);

            }
            else
            {


                glDrawElements(GL_TRIANGLES, pFace.numMeshVerts,
                               GL_UNSIGNED_INT,&P_INDICES[pFace.meshVertIndex]);

            }

        }
        else if(pFace.lightmapIndex != -1)
        { // Lightmap



            glTexCoordPointer(2,
                              GL_FLOAT,sizeof(Q3BSPVertex),&(P_VERTS[pFace.startVertIndex].lightmapCoord));

            glActiveTextureARB (GL_TEXTURE0_ARB);
            glEnable (GL_TEXTURE_2D);

            if(lastTexture1 != LIGHTMAPS[pFace.lightmapIndex])
            {
                lastTexture1 = LIGHTMAPS[pFace.lightmapIndex];
                glBindTexture (GL_TEXTURE_2D, lastTexture1);

            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);

            glDrawElements(GL_TRIANGLES, pFace.numMeshVerts,
                           GL_UNSIGNED_INT,&P_INDICES[pFace.meshVertIndex]);


        }
    }

    // @@@ TODO this should not be required!!
    CGLState::setDefaultState();
    
    //glUnlockArraysEXT_p();
}

// @@@ Do shaders for patches
void
Q3BSPRender::RenderPatch (int faceIndex)
{

    if (P_PATCHES[faceIndex] == NULL)
        CreatePatch (faceIndex);

    Q3BSPFace *pFace = &P_FACES[faceIndex];

    glActiveTextureARB (GL_TEXTURE0_ARB);

    if(lastTexture1 != TEXTURES[pFace->textureIndex])
    {
        // Turn on texture mapping and bind the face's texture map
        glEnable (GL_TEXTURE_2D);
        lastTexture1 = TEXTURES[pFace->textureIndex];
        glBindTexture (GL_TEXTURE_2D, lastTexture1);

    }

    // Set the current pass as the second lightmap texture_
    glActiveTextureARB (GL_TEXTURE1_ARB);

    if(lastTexture2 != LIGHTMAPS[pFace->lightmapIndex])
    {
        // Turn on texture mapping and bind the face's lightmap over the texture
        glEnable (GL_TEXTURE_2D);
        lastTexture2 = LIGHTMAPS[pFace->lightmapIndex];
        glBindTexture (GL_TEXTURE_2D, lastTexture2);
    }

    for (int i = 0; i < P_PATCHES[faceIndex]->iSubPatches; i++)
    {

        glClientActiveTextureARB (GL_TEXTURE1_ARB);

        P_PATCHES[faceIndex]->pVB[i].SetTexCoordPointer (2);

        glClientActiveTextureARB (GL_TEXTURE0_ARB);

        P_PATCHES[faceIndex]->pVB[i].SetTexCoordPointer (1);

        P_PATCHES[faceIndex]->pVB[i].SetPointers ();

        glDrawArrays (GL_TRIANGLE_STRIP, 0,
                      P_PATCHES[faceIndex]->pVB->GetSize ());

    }



}

int
Q3BSPRender::findLeaf(const Vec3f & pos) const
{
	int i = 0;

	// This function takes in our camera position, then goes and walks
	// through the BSP nodes, starting at the root node, finding the leaf node
	// that our camera resides in.  This is done by checking to see if
	// the camera is in front or back of each node's splitting plane.
	// If the camera is in front of the camera, then we want to check
	// the node in front of the node just tested.  If the camera is behind
	// the current node, we check that nodes back node.  Eventually, this
	// will find where the camera is according to the BSP tree.  Once a
	// node index (i) is found to be a negative number, that tells us that
	// that index is a leaf node, not another BSP node.  We can either calculate
	// the leaf node index from -(i + 1) or ~1.  This is because the starting
	// leaf index is 0, and you can't have a negative 0.  It's important
	// for us to know which leaf our camera is in so that we know which cluster
	// we are in.  That way we can test if other clusters are seen from our cluster.

	// Continue looping until we find a negative index
	while (i >= 0) {
		// Get the current node, then find the slitter plane from that
		// node's plane index.  Notice that we use a constant reference
		// to store the plane and node so we get some optimization.
		const Q3BSPNode & node= P_NODES[i];
		const Plane3f & plane= P_PLANES[node.planeIndex];

		// If the pos is in front of the plane
		if (plane.getDistanceFromPlane(pos) >= 0) {
			// Assign the current node to the node in front of itself
			i = node.frontChildIndex;
		}
		// Else if the camera is behind the plane
		else {
			// Assign the current node to the node behind itself
			i = node.backChildIndex;
		}
	}

	//lastCamLeaf = ~i;

	// Return the leaf index (same thing as saying:  return -(i + 1)).
	return ~i; // Binary operation
}

void
Q3BSPRender::renderMap ()
{

    int i;

    updateAnimationMaps();

    updateTCMods();

    int leafIndex = findLeaf(cam->getPosition());

    // Get the cluster the camera is in.
    camClusterIndex = P_LEAFS[leafIndex].visibilityClusterIndex;

    for(i = 0; i < NUM_OF_LEAFS; ++i)
    {

        const Q3BSPLeaf & leaf = P_LEAFS[i];

        // If the current leaf can't be seen from our cluster, go to the next leaf
        if (!IsClusterVisible (camClusterIndex, leaf.visibilityClusterIndex))
        {
            continue;
        }

        // If the current leaf is not in the camera's frustum, go to the next leaf
        if (!AABBInFrustum (leaf.boundingMin, leaf.boundingMax))
        {
            continue;
        }

        // If we get here, the leaf we are testing must be visible in our camera's view.
        // Get the number of faces that this leaf is in charge of.
        int faceCount = leaf.numLeafFaces;

        // Loop through and render all of the faces in this leaf
        while (faceCount--)
        {
            // Grab the current face index from our leaf faces array

            int faceIndex =
                P_LEAF_FACES[leaf.leafFaceIndex + faceCount];

            m_FacesDrawn.Set (faceIndex);
        }


        vector < int >::const_iterator j;

        for(j = P_LEAF_MODELS[i].begin(); j != P_LEAF_MODELS[i].end(); ++j)
        {
            const Q3BSPModel & model = P_MODELS[*j];

            for(int k = 0; k < model.numFaces; ++k)
            {
                m_FacesDrawn.Set(model.firstFaceIndex + k);
            }
        }


    }

    glClientActiveTextureARB (GL_TEXTURE1_ARB);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glEnableClientState (GL_VERTEX_ARRAY);

    glClientActiveTextureARB (GL_TEXTURE0_ARB);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    // Set our vertex array client states for vertices and texture coordinates
    glEnableClientState (GL_VERTEX_ARRAY);

    CGLState::setDefaultState();

    for(i = 0; i < v_normalFaces.size(); ++i)
    {

        const int faceIndex = v_normalFaces[i];
        if(m_FacesDrawn.On(faceIndex))
        {

            switch (P_FACES[faceIndex].type & 0x0000000F)
            {
            case FACE_POLYGON:


                if(renderNormalFaces)
                {
                    RenderNormalFace (faceIndex) ;
                }

                break;
            case FACE_PATCH:
                if(renderPatches)
                {
                    RenderPatch (faceIndex) ;
                }
                break;
            case FACE_MESH:
                if(renderNormalFaces)
                {
                    RenderNormalFace (faceIndex) ;
                }
                break;
            case FACE_BILLBOARD:
                break;
            default:
                cout << "@@@" << endl;
                break;
            }


        }

    }

    glActiveTextureARB (GL_TEXTURE1_ARB);
    glDisable (GL_TEXTURE_2D);


    if(renderOpaqueFaces)
    {
        for(i = 0; i < v_opaqueFaces.size(); ++i)
        {
            const int faceIndex = v_opaqueFaces[i];
            if(m_FacesDrawn.On(faceIndex))
            {
                RenderFace(faceIndex);
            }
        }
    }

    if(renderAlphaHoleNoBlendingFaces)
    {
        for(i = 0; i < v_alphaHoleNoBlendingFaces.size(); ++i)
        {
            const int faceIndex = v_alphaHoleNoBlendingFaces[i];
            if(m_FacesDrawn.On(faceIndex))
            {
                RenderFace(faceIndex);
            }
        }
    }

    if(renderAlphaAndBlendingFaces)
    {
        for(i = 0; i < v_alphaAndBlendingFaces.size(); ++i)
        {
            const int faceIndex = v_alphaAndBlendingFaces[i];
            if(m_FacesDrawn.On(faceIndex))
            {
                RenderFace(faceIndex);
            }
        }
    }

    if(renderBlendingOnlyFaces)
    {
        for(i = 0; i < v_blendingOnlyFaces.size(); ++i)
        {
            const int faceIndex = v_blendingOnlyFaces[i];
            if(m_FacesDrawn.On(faceIndex))
            {
                RenderFace(faceIndex);
            }
        }
    }

    m_FacesDrawn.ClearAll ();

}

void
Q3BSPRender::initFaceVectors()
{
    v_normalFaces.clear();
    v_opaqueFaces.clear();
    v_alphaHoleNoBlendingFaces.clear();
    v_alphaAndBlendingFaces.clear();
    v_blendingOnlyFaces.clear();

    int i;

    cout << "NUM_OF_FACES = " << NUM_OF_FACES << endl;

    for(i = 0; i < NUM_OF_FACES; ++i)
    {
        Q3BSPFace & face = P_FACES[i];

        switch(classifyFaceType(i))
        {
        case FACE_TYPE_NORMAL:
            v_normalFaces.push_back(i);
            face.type |= FACE_TYPE_NORMAL;

            break;

        case FACE_TYPE_OPAQUE:
            v_opaqueFaces.push_back(i);
            face.type |= FACE_TYPE_OPAQUE;
            break;

        case FACE_TYPE_ALPHA_HOLE_NO_BLENDING:
            v_alphaHoleNoBlendingFaces.push_back(i);

            face.type |= FACE_TYPE_ALPHA_HOLE_NO_BLENDING;
            break;

        case FACE_TYPE_ALPHA_AND_BLENDING:
            v_alphaAndBlendingFaces.push_back(i);

            face.type |= FACE_TYPE_ALPHA_AND_BLENDING;
            break;

        case FACE_TYPE_BLENDING_ONLY:
            v_blendingOnlyFaces.push_back(i);
            face.type |= FACE_TYPE_BLENDING_ONLY;
            break;

        }
    }
    cout << "v_normalFaces.size() = " << v_normalFaces.size() << endl;
    cout << "v_opaqueFaces.size() = " << v_opaqueFaces.size() << endl;
    cout << "v_alphaHoleNoBlendingFaces.size() = " << v_alphaHoleNoBlendingFaces.size() << endl;
    cout << "v_alphaAndBlendingFaces.size() = " << v_alphaAndBlendingFaces.size() << endl;
    cout << "v_blendingOnlyFaces.size() = " << v_blendingOnlyFaces.size() << endl;



	if(v_normalFaces.size() > 2) { quickSort(v_normalFaces, 0, v_normalFaces.size() - 1) ;}
    if(v_opaqueFaces.size() > 2) { quickSort(v_opaqueFaces, 0, v_opaqueFaces.size() - 1) ;}
    if(v_alphaHoleNoBlendingFaces.size() > 2)
    {
        quickSort(v_alphaHoleNoBlendingFaces, 0,
                  v_alphaHoleNoBlendingFaces.size() - 1) ;
    }
    if(v_alphaAndBlendingFaces.size() > 2) { quickSort(v_alphaAndBlendingFaces, 0, v_alphaAndBlendingFaces.size() -1) ;}
    if(v_blendingOnlyFaces.size() > 2) { quickSort(v_blendingOnlyFaces, 0, v_blendingOnlyFaces.size() - 1) ;}


    /*
    for(i = 0; i < v_normalFaces.size(); ++i)
    {
        const Q3BSPFace & face = P_FACES[v_normalFaces[i]];

        cout << face.textureIndex << " " << face.lightmapIndex << endl;
    }
    */

    /*
    for(i = 0; i < v_opaqueFaces.size(); ++i)
    {
        const Q3BSPFace & face = P_FACES[v_opaqueFaces[i]];
        cout << face.textureIndex << endl;
    }
    */

}

int
Q3BSPRender::classifyFaceType(int faceIndex)
{
    const Q3BSPFace & face = P_FACES[faceIndex];

    if(P_TEXTURES[face.textureIndex].shader != -1)
    {
        Q3BSPShader & shader = P_SHADERS[P_TEXTURES[face.textureIndex].shader];

        int i;

        // Search for at least one opaque layer
        for(i = (int)shader.layers->numOfLayers - 1; i >= 0; --i) // correct order
        {
            const Q3BSPShaderLayer & layer = shader.layers->layers[i];

            if(layer.texture && (layer.blendingParms == BLEND_NO_BLENDING)
                    && (layer.alphaFunc == ALPHA_FUNC_NO_ALPHA))
            {
                return FACE_TYPE_OPAQUE;
            }
        }

        // Alpha faces, but with no blending faces with Depth Buffer in read/write mode (Wire mesh,
        // spinning fans)
        bool gotAlphaHole = false;
        bool gotBlending = false;

        for(i = (int)shader.layers->numOfLayers - 1; i >= 0; --i) // correct order
        {
            const Q3BSPShaderLayer & layer = shader.layers->layers[i];
            
            if(layer.texture)
            {

                if(layer.alphaFunc != ALPHA_FUNC_NO_ALPHA)
                {
                    gotAlphaHole = true;
                }

                if(layer.blendingParms != BLEND_NO_BLENDING)
                {
                    gotBlending = true;
                }

            }
        }

        if(gotAlphaHole && !gotBlending) { return FACE_TYPE_ALPHA_HOLE_NO_BLENDING ;}
        if(gotAlphaHole &&  gotBlending) { return FACE_TYPE_ALPHA_AND_BLENDING ;}

        //completely blended faces, sparks, partials, etc with depth buffer in read only mode.
        return FACE_TYPE_BLENDING_ONLY;

    }
    else
    {
        return FACE_TYPE_NORMAL;
    }

    return 0;
}

// @@@ Move to a sorting class

void
Q3BSPRender::quickSort(vector<int> & array, int left, int right)
{
    // Pick pivot
    unsigned int pivot = (P_FACES[array[right]].textureIndex << 8) | (P_FACES[array[right]].lightmapIndex);

    leftMark = left - 1;
    rightMark = right;

    // Partition the current sub array
    while(true)
    {

        // Find which leftMark to swap
        while(leftMark < right && (unsigned int)((P_FACES[array[++leftMark]].textureIndex << 8)
            | (P_FACES[array[leftMark]].lightmapIndex)) < pivot) {  ;}

        // Find which rightMark to swap
        while(rightMark > 0 && (unsigned int)((P_FACES[array[--rightMark]].textureIndex << 8)
                                          | (P_FACES[array[rightMark]].lightmapIndex)) > pivot) {  ;}

        // Done swapping
        if(leftMark >= rightMark) { break ;}

        // Swap leftMark and rightMark so that the leftMark is
        // <= then the pivot and the rightMark is >= pivot
        else
        { //mySwap ( leftMark, rightMark ) ;}

            tmp = array[leftMark];

            array[leftMark] = array[rightMark];

            array[rightMark] = tmp ;
        }

    }

    // Put the pivot where it should be

    tmp = array[leftMark];

    array[leftMark] = array[right];

    array[right] = tmp;

    // Base case, check to see if we need to sort the left side of the subarray
    if((leftMark - 1) - left > 0)
    {
        // Sort to the left
        quickSort(array, left, leftMark - 1);
    }

    // Base case, check to see if we need to sort the right side of the subarray
    if(right - (leftMark + 1) > 0)
    {
        // Sort to the right
        quickSort(array, leftMark + 1, right);
    }
}

void
Q3BSPRender::updateAnimationMaps()
{
    vector < Q3AnimMap * >::iterator i;

    // @@@ This need to wrap!
    currTime += dTime;

    for(i = map->animationMaps.begin(); i != map->animationMaps.end(); ++i)
    {
        Q3AnimMap * animMap = *i;

        animMap->currFrame = ((int) floor(currTime* animMap->frequency)) % animMap->numOfFrames;

    }

}

/*
Now that you are reading this, then you should be reminded to create a birds eye render. That 
just renders the nav graph. perhaps your could create an algorithm that combines trianges to create convex polygons, so that the rendering would be faster. 
 
Then as you knwo with q3dm13 with 60 bots a big cluster fuck happens. Find an algorithm to detect cluster fucks and rate them. Then find the bots that are on the outside of the cluster fuck and have them find another destination. 
*/

