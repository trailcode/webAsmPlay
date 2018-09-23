#include <tceGeom/Q3BSPBezierPatch.h>
#include <tceGeom/Q3BSPMap.h>
#include <tceRender/vertexBuffer.h>

using namespace tce::geom;
using namespace tce::geom::quake3;

Q3BSPBezierPatch::Q3BSPBezierPatch()
{
	// TODO: put constructor code here
}

Q3BSPBezierPatch::Q3BSPBezierPatch(const Q3BSPBezierPatch & patch)
{
    // TODO
}

Q3BSPBezierPatch::~Q3BSPBezierPatch()
{
	// TODO: put destructor code here
}

Q3BSPBezierPatch & 
Q3BSPBezierPatch::operator=(const Q3BSPBezierPatch & patch)
{
    // TODO
    return *this;
}

void
Q3BSPBezierPatch::CreatePatch (int iFaceIndex)
{
	int LevelToWidth[] = { 3, 5, 9, 17, 33, 65, 129 };

	Q3BSPFace *pFace = &P_FACES[iFaceIndex];

	//Vec3f *cp = &m_pVB->GetVertices()[pFace->startVertIndex];
	//CVector2 *cptc = &m_pVB->GetTexCoords()[pFace->startVertIndex];
	//CVector2 *cptc2 = &m_pVB->GetTexCoords2()[pFace->startVertIndex];

	int numPatchesW, numPatchesL, gy, gx;

	Vec3f temp[3][3];
	Vec2f temptc[3][3];
	Vec2f temptc2[3][3];

	numPatchesW = (pFace->size[0] - 1) / 2;	// @@@ size[] could be lMapSize[]
	numPatchesL = (pFace->size[1] - 1) / 2;


	BSPPatch *pPatch = new BSPPatch;

	P_PATCHES[iFaceIndex] = pPatch;

	pPatch->iSubPatches = numPatchesL * numPatchesW * (LevelToWidth[2] - 1);
	pPatch->iSizes = new int[pPatch->iSubPatches];
	pPatch->pVB = new VertexBuffer[pPatch->iSubPatches];

	int iSubPatch = 0;

	for (unsigned int hp = 0; hp < numPatchesL; hp++)
	{

		gy = 2 * hp;	//y start location of patch

		for (unsigned int wp = 0; wp < numPatchesW; wp++)
		{
			gx = 2 * wp;	// x start location of patch

			int i1 = ((gy + 0) * pFace->size[0]) + gx;
			int i2 = ((gy + 1) * pFace->size[0]) + gx;
			int i3 = ((gy + 2) * pFace->size[0]) + gx;

			//Grab the control points for this patch

			temp[0][0] = P_VERTS[pFace->startVertIndex + i1 + 0].position;
			temp[0][1] = P_VERTS[pFace->startVertIndex + i1 + 1].position;
			temp[0][2] = P_VERTS[pFace->startVertIndex + i1 + 2].position;
			temp[1][0] = P_VERTS[pFace->startVertIndex + i2 + 0].position;
			temp[1][1] = P_VERTS[pFace->startVertIndex + i2 + 1].position;
			temp[1][2] = P_VERTS[pFace->startVertIndex + i2 + 2].position;
			temp[2][0] = P_VERTS[pFace->startVertIndex + i3 + 0].position;
			temp[2][1] = P_VERTS[pFace->startVertIndex + i3 + 1].position;
			temp[2][2] = P_VERTS[pFace->startVertIndex + i3 + 2].position;


			//Grab the control points tex coords for this patch

			temptc[0][0] = P_VERTS[pFace->startVertIndex + i1].textureCoord;
			//temptc[0][0].y *= -1;


			temptc[0][1] = P_VERTS[pFace->startVertIndex + i1 + 1].textureCoord;
			//temptc[0][1].y *= -1;


			temptc[0][2] = P_VERTS[pFace->startVertIndex + i1 + 2].textureCoord;
			//temptc[0][2].y *= -1;


			temptc[1][0] = P_VERTS[pFace->startVertIndex + i2].textureCoord;
			//temptc[1][0].y *= -1;


			temptc[1][1] = P_VERTS[pFace->startVertIndex + i2 + 1].textureCoord;
			//temptc[1][1].y *= -1;


			temptc[1][2] = P_VERTS[pFace->startVertIndex + i2 + 2].textureCoord;
			//temptc[1][2].y *= -1;


			temptc[2][0] = P_VERTS[pFace->startVertIndex + i3].textureCoord;
			//temptc[2][0].y *= -1;


			temptc[2][1] = P_VERTS[pFace->startVertIndex + i3 + 1].textureCoord;
			//temptc[2][1].y *= -1;


			temptc[2][2] = P_VERTS[pFace->startVertIndex + i3 + 2].textureCoord;
			//temptc[2][2].y *= -1;

			//Grab the control points tex coords for this patch


			temptc2[0][0] = P_VERTS[pFace->startVertIndex + i1].lightmapCoord;
			//temptc2[0][0].y *= -1;


			temptc2[0][1] = P_VERTS[pFace->startVertIndex + i1 + 1].lightmapCoord;
			//temptc2[0][1].y *= -1;


			temptc2[0][2] = P_VERTS[pFace->startVertIndex + i1 + 2].lightmapCoord;
			//temptc2[0][2].y *= -1;


			temptc2[1][0] = P_VERTS[pFace->startVertIndex + i2].lightmapCoord;
			//temptc2[1][0].y *= -1;


			temptc2[1][1] = P_VERTS[pFace->startVertIndex + i2 + 1].lightmapCoord;
			//temptc2[1][1].y *= -1;


			temptc2[1][2] = P_VERTS[pFace->startVertIndex + i2 + 2].lightmapCoord;
			//temptc2[1][2].y *= -1;


			temptc2[2][0] = P_VERTS[pFace->startVertIndex + i3].lightmapCoord;
			//temptc2[2][0].y *= -1;


			temptc2[2][1] = P_VERTS[pFace->startVertIndex + i3 + 1].lightmapCoord;
			//temptc2[2][1].y *= -1;


			temptc2[2][2] = P_VERTS[pFace->startVertIndex + i3 + 2].lightmapCoord;
			//temptc2[2][2].y *= -1;


			Vec3f *pPoints = QuadraticBezierPatchSubdivide2 (temp, 2);
			
			Vec2f *pTexCoords = QuadraticBezierPatchSubdivide1 (temptc, 2);
			
			Vec2f *pTexCoords2 = QuadraticBezierPatchSubdivide1 (temptc2, 2);

			int width = LevelToWidth[2];

			for (int i = 0; i < width - 1; i++)
			{

				pPatch->iSizes[iSubPatch] = 2 * width;
				
				pPatch->pVB[iSubPatch].AllocBuffer (pPatch->iSizes[iSubPatch]);

				for (int j = 0; j < width; j++)
				{
					pPatch->pVB[iSubPatch].GetVertices   ()[j * 2] = pPoints    [(i + 1) * width + j];
					pPatch->pVB[iSubPatch].GetTexCoords  ()[j * 2] = pTexCoords [(i + 1) * width + j];
					pPatch->pVB[iSubPatch].GetTexCoords2 ()[j * 2] = pTexCoords2[(i + 1) * width + j];
					
					pPatch->pVB[iSubPatch].GetVertices   ()[j * 2 + 1] = pPoints    [i * width + j];
					pPatch->pVB[iSubPatch].GetTexCoords  ()[j * 2 + 1] = pTexCoords [i * width + j];
					pPatch->pVB[iSubPatch].GetTexCoords2 ()[j * 2 + 1] = pTexCoords2[i * width + j];
				}

				iSubPatch++;
			}

			delete [] pTexCoords;
			delete [] pTexCoords2;
			delete [] pPoints;
		}
	}
}

// Some Utility functions

Vec2f *
Q3BSPBezierPatch::QuadraticBezierPatchSubdivide1 (Vec2f G[][3], int iLevel)
{

	int LevelToWidth[] = { 3, 5, 9, 17, 33, 65, 129 };

	// Calculate some indexes

	int iWidth = LevelToWidth[iLevel];
	int iMid = iWidth >> 1;
	int iTotal = iWidth * iWidth;

	// Allocate space for the points on the surface and the interpolation of the
	// control points
	Vec2f *p = new Vec2f[iTotal + 3 * iWidth];

	// Interpolate the control points using the geometry matrix
	int i;

	for (i = 0; i < 3; i++)
	{

		// Distribute the control points into the space to interpolate into

		p[iTotal] = G[i][0];
		p[iTotal + iMid] = G[i][1];
		p[iTotal + iWidth - 1] = G[i][2];

		// Interpolate the control points

		QuadraticBezierSubdivide1 (&p[iTotal], iLevel);

		// Distribute the interpolated control points into the allocated point space

		Vec2f *index = p + i * iMid;

		for (int j = 0; j < iWidth; j++)
		{

			*index = p[iTotal + j];
			index += iWidth;

		}

		iTotal += iWidth;

	}

	// Interpolate all of the remaining points

	for (i = 0; i < iWidth; i++)
	{

		QuadraticBezierSubdivide1 (&p[i * iWidth], iLevel);

	}

	// Return the points. Must be freed by the caller!

	return p;

}



Vec3f *
Q3BSPBezierPatch::QuadraticBezierPatchSubdivide2 (Vec3f G[][3], int iLevel)
{

	int LevelToWidth[] = { 3, 5, 9, 17, 33, 65, 129 };

	// Calculate some indexes

	int iWidth = LevelToWidth[iLevel];
	int iMid = iWidth >> 1;
	int iTotal = iWidth * iWidth;

	// Allocate space for the points on the surface and the interpolation of the

	// control points

	Vec3f *p = new Vec3f[iTotal + 3 * iWidth];

	// Interpolate the control points using the geometry matrix
	int i;
	for (i = 0; i < 3; i++)
	{

		// Distribute the control points into the space to interpolate into

		p[iTotal] = G[i][0];
		p[iTotal + iMid] = G[i][1];
		p[iTotal + iWidth - 1] = G[i][2];

		// Interpolate the control points

		QuadraticBezierSubdivide2 (&p[iTotal], iLevel);

		// Distribute the interpolated control points into the allocated point space

		Vec3f *index = p + i * iMid;

		for (int j = 0; j < iWidth; j++)
		{

			*index = p[iTotal + j];
			index += iWidth;

		}

		iTotal += iWidth;

	}

	// Interpolate all of the remaining points

	for (i = 0; i < iWidth; i++)
	{

		QuadraticBezierSubdivide2 (&p[i * iWidth], iLevel);

	}

	// Return the points. Must be freed by the caller!

	return p;

}



void
Q3BSPBezierPatch::QuadraticBezierSubdivide1 (Vec2f G[], int iLevel)
{

	int LevelToWidth[] = { 3, 5, 9, 17, 33, 65, 129 };

	// Do not subdivide any more, however, move middle control point to be
	// a point on the curve

	if (iLevel == 0)
	{

		Vec2f a = (G[0] + G[1]) * 0.5f;
		Vec2f b = (G[1] + G[2]) * 0.5f;
		G[1] = (a + b) * 0.5f;
		return;

	}

	// Calculate subdivided indexes

	int iWidth = LevelToWidth[iLevel] - 1;
	int iMid = iWidth >> 1;
	int a = iWidth >> 2;
	int b = iMid + a;

	// Enter subdivided control points into the geometry vector

	G[a] = (G[0] + G[iMid]) * 0.5f;
	G[b] = (G[iMid] + G[iWidth]) * 0.5f;
	G[iMid] = (G[a] + G[b]) * 0.5f;

	// Call recursively for left and right halves

	QuadraticBezierSubdivide1 (G, --iLevel);
	QuadraticBezierSubdivide1 (&G[iMid], iLevel);

}


void
Q3BSPBezierPatch::QuadraticBezierSubdivide2 (Vec3f G[], int iLevel)
{

	int LevelToWidth[] = { 3, 5, 9, 17, 33, 65, 129 };


	// Do not subdivide any more, however, move middle control point to be
	// a point on the curve

	if (iLevel == 0)
	{

		Vec3f a = (G[0] + G[1]) * 0.5f;
		Vec3f b = (G[1] + G[2]) * 0.5f;
		G[1] = (a + b) * 0.5f;
		return;

	}

	// Calculate subdivided indexes

	int iWidth = LevelToWidth[iLevel] - 1;
	int iMid = iWidth >> 1;
	int a = iWidth >> 2;
	int b = iMid + a;

	// Enter subdivided control points into the geometry vector

	G[a] = (G[0] + G[iMid]) * 0.5f;
	G[b] = (G[iMid] + G[iWidth]) * 0.5f;
	G[iMid] = (G[a] + G[b]) * 0.5f;

	// Call recursively for left and right halves

	QuadraticBezierSubdivide2 (G, --iLevel);
	QuadraticBezierSubdivide2 (&G[iMid], iLevel);

}
