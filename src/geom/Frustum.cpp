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

void Frustum::set(const dmat4 & modelViewProjection)
{
	const double * mvp = (const double *)value_ptr(modelViewProjection);

	/*
	MVP[0][0] 0 ,MVP[0][1] 1 ,MVP[0][2] 2 ,MVP[0][3] 3,
	MVP[1][0] 4 ,MVP[1][1] 5 ,MVP[1][2] 6 ,MVP[1][3] 7,
	MVP[2][0] 8 ,MVP[2][1] 9 ,MVP[2][2] 10,MVP[2][3] 11,
	MVP[3][0] 12,MVP[3][1] 13,MVP[3][2] 14,MVP[3][3]) 15
	*/

	//const float

	planes[0] = Plane(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]);
	planes[1] = Plane(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
	planes[2] = Plane(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
	planes[3] = Plane(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
	planes[4] = Plane(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
	planes[5] = Plane(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);

	/*
	planes[RIGHT_CLIP_PLANE]  = Plane3d(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]);
	planes[LEFT_CLIP_PLANE]   = Plane3d(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
	planes[BOTTOM_CLIP_PLANE] = Plane3d(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
	planes[TOP_CLIP_PLANE]    = Plane3d(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
	planes[FAR_CLIP_PLANE]    = Plane3d(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
	planes[NEAR_CLIP_PLANE]   = Plane3d(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);
	*/

	for(size_t i = 0; i < 6; ++i) { planes[i].invertNormal() ;} // Do this above!
																//for(size_t i = 0; i < 6; ++i) { planes[i].invert() ;} // Do this above!
}

bool Frustum::AABBInFrustum(const AABB3D & AABB) const
{
	for(size_t i = 0; i < 6; ++i)
	{
		//if(AABB.classify(planes[i]) == SIDE_BACK) { return false ;}
		if(AABB.classify(planes[i]) == 1) { return false ;}
	}

	return true;
}


