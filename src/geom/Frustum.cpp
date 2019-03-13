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
\copyright 2019
*/

#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/geom/Frustum.h>

using namespace glm;

Frustum::Frustum()
{

}

Frustum::Frustum(const dmat4 & modelViewProjection)
{
	set(modelViewProjection);
}

struct Matrix4x4
{
	// The elements of the 4x4 matrix are stored in
	// column-major order (see "OpenGL Programming Guide",
	// 3rd edition, pp 106, glLoadMatrix).
	double _11, _21, _31, _41;
	double _12, _22, _32, _42;
	double _13, _23, _33, _43;
	double _14, _24, _34, _44;
};

void Frustum::set(const dmat4 & modelViewProjection)
{
	const double * mvp = (const double *)value_ptr(modelViewProjection);

	Matrix4x4 comboMatrix;

	memcpy(&comboMatrix, mvp, sizeof(Matrix4x4));

	/*
	MVP[0][0] 0 ,MVP[0][1] 1 ,MVP[0][2] 2 ,MVP[0][3] 3,
	MVP[1][0] 4 ,MVP[1][1] 5 ,MVP[1][2] 6 ,MVP[1][3] 7,
	MVP[2][0] 8 ,MVP[2][1] 9 ,MVP[2][2] 10,MVP[2][3] 11,
	MVP[3][0] 12,MVP[3][1] 13,MVP[3][2] 14,MVP[3][3]) 15
	*/

	/*
	planes[0] = Plane(comboMatrix._41 + comboMatrix._11, comboMatrix._42 + comboMatrix._12, comboMatrix._43 + comboMatrix._13, comboMatrix._44 + comboMatrix._14);
	planes[1] = Plane(comboMatrix._41 - comboMatrix._11, comboMatrix._42 - comboMatrix._12, comboMatrix._43 - comboMatrix._13, comboMatrix._44 - comboMatrix._14);
	planes[2] = Plane(comboMatrix._41 - comboMatrix._21, comboMatrix._42 - comboMatrix._22, comboMatrix._43 - comboMatrix._23, comboMatrix._44 - comboMatrix._24);
	planes[3] = Plane(comboMatrix._41 + comboMatrix._21, comboMatrix._42 + comboMatrix._22, comboMatrix._43 + comboMatrix._23, comboMatrix._44 + comboMatrix._24);
	planes[4] = Plane(comboMatrix._41 + comboMatrix._31, comboMatrix._42 + comboMatrix._32, comboMatrix._43 + comboMatrix._33, comboMatrix._44 + comboMatrix._34);	planes[5] = Plane(comboMatrix._41 - comboMatrix._31, comboMatrix._42 - comboMatrix._32, comboMatrix._43 - comboMatrix._33, comboMatrix._44 - comboMatrix._34);	//*/
	/*
	planes[0] = Plane(comboMatrix._14 + comboMatrix._11, comboMatrix._24 + comboMatrix._21, comboMatrix._34 + comboMatrix._31, comboMatrix._44 + comboMatrix._41);
	planes[1] = Plane(comboMatrix._14 - comboMatrix._11, comboMatrix._24 - comboMatrix._21, comboMatrix._34 - comboMatrix._31, comboMatrix._44 - comboMatrix._41);
	planes[3] = Plane(comboMatrix._14 - comboMatrix._12, comboMatrix._24 - comboMatrix._22, comboMatrix._34 - comboMatrix._32, comboMatrix._44 - comboMatrix._42);
	planes[4] = Plane(comboMatrix._14 + comboMatrix._12, comboMatrix._24 + comboMatrix._22, comboMatrix._34 + comboMatrix._32, comboMatrix._44 + comboMatrix._42);
	planes[5] = Plane(comboMatrix._13, comboMatrix._23, comboMatrix._33, comboMatrix._43);
	planes[6] = Plane(comboMatrix._14 - comboMatrix._13, comboMatrix._24 - comboMatrix._23, comboMatrix._34 - comboMatrix._33, comboMatrix._44 - comboMatrix._43);
	*/

	/*
	planes[0] = Plane(comboMatrix._14 + comboMatrix._11, comboMatrix._24 + comboMatrix._21, comboMatrix._34 + comboMatrix._31, comboMatrix._44 + comboMatrix._41);
	planes[1] = Plane(comboMatrix._14 - comboMatrix._11, comboMatrix._24 - comboMatrix._21, comboMatrix._34 - comboMatrix._31, comboMatrix._44 - comboMatrix._41);
	planes[3] = Plane(comboMatrix._14 - comboMatrix._12, comboMatrix._24 - comboMatrix._22, comboMatrix._34 - comboMatrix._32, comboMatrix._44 - comboMatrix._42);
	planes[4] = Plane(comboMatrix._14 + comboMatrix._12, comboMatrix._24 + comboMatrix._22, comboMatrix._34 + comboMatrix._32, comboMatrix._44 + comboMatrix._42);
	planes[4] = Plane(comboMatrix._14 + comboMatrix._13, comboMatrix._24 + comboMatrix._23, comboMatrix._34 + comboMatrix._33, comboMatrix._44 + comboMatrix._43);
	planes[6] = Plane(comboMatrix._14 - comboMatrix._13, comboMatrix._24 - comboMatrix._23, comboMatrix._34 - comboMatrix._33, comboMatrix._44 - comboMatrix._43);
	*/

	//const float
	//*
	planes[0] = Plane(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]); // This one is correct!
	planes[1] = Plane(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
	planes[2] = Plane(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
	planes[3] = Plane(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
	planes[4] = Plane(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
	planes[5] = Plane(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);
	//*/

	/*
	planes[RIGHT_CLIP_PLANE]  = Plane3d(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]);
	planes[LEFT_CLIP_PLANE]   = Plane3d(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
	planes[BOTTOM_CLIP_PLANE] = Plane3d(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
	planes[TOP_CLIP_PLANE]    = Plane3d(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
	planes[FAR_CLIP_PLANE]    = Plane3d(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
	planes[NEAR_CLIP_PLANE]   = Plane3d(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);
	*/

	//for(size_t i = 0; i < 6; ++i) { planes[i].invertNormal() ;} // Do this above!
																//for(size_t i = 0; i < 6; ++i) { planes[i].invert() ;} // Do this above!
}

bool Frustum::AABBInFrustum(const AABB3D & AABB) const
{
	for(size_t i = 0; i < 6; ++i)
	{
		if(AABB.classify(planes[i]) == Plane::SIDE_BACK) { return false ;} // DOES not seem to work
		//if(AABB.classify(planes[i]) == 1) { return false ;}
	}

	return true;
}

bool Frustum::intersectsEdge(const dvec3& P1, const dvec3& P2) const
{
	if(containsPoint(P1) || containsPoint(P2)) { return true ;}
	
	size_t c = 0;
	
	for (size_t i = 0; i < 6; ++i)
	{
		if(planes[i].classifyPoint(P1) != planes[i].classifyPoint(P2))
		{
			++c;
		
			if(c > 1) { return true ;}
		}
	}

	return c > 1;
	//return c == 2;
}

bool Frustum::intersects(const dvec3& P1, const dvec3& P2, const dvec3& P3, const dvec3& P4) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if(	planes[i].classifyPoint(P1) == Plane::SIDE_BACK &&
			planes[i].classifyPoint(P2) == Plane::SIDE_BACK &&
			planes[i].classifyPoint(P3) == Plane::SIDE_BACK &&
			planes[i].classifyPoint(P4) == Plane::SIDE_BACK) { return false ;}
	}

	return true;
}

//int frustumAABBIntersect(const AABB3D & AABB) const

bool Frustum::containsPoint(const dvec3 & P) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if(planes[i].classifyPoint(P) == Plane::SIDE_BACK) { return false ;}
	}

	return true;
}


bool Frustum::containsA_Point(const dvec3 & P1, const dvec3 & P2, const dvec3 & P3, const dvec3 & P4) const
{
	return	containsPoint(P1) ||
			containsPoint(P2) ||
			containsPoint(P3) ||
			containsPoint(P4);
}

const Plane & Frustum::getPlane(const size_t index) const { return planes[index] ;}


