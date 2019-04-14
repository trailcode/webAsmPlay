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

Frustum::Frustum() {}

Frustum::Frustum(const dmat4 & modelViewProjection) { set(modelViewProjection) ;}

void Frustum::set(const dmat4 & modelViewProjection)
{
	const double * mvp = (const double *)value_ptr(modelViewProjection);

	m_planes[0] = Plane(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]);
	m_planes[1] = Plane(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
	m_planes[2] = Plane(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
	m_planes[3] = Plane(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
	m_planes[4] = Plane(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
	m_planes[5] = Plane(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);
}

bool Frustum::AABBInFrustum(const AABB3D & AABB) const
{
	for(size_t i = 0; i < 6; ++i)
	{
		if(AABB.classify(m_planes[i]) == Plane::SIDE_BACK) { return false ;}
	}

	return true;
}

bool Frustum::intersectsEdge(const dvec3& P1, const dvec3& P2) const
{
	if(containsPoint(P1) || containsPoint(P2)) { return true ;}
	
	size_t c = 0;
	
	for (size_t i = 0; i < 6; ++i)
	{
		if(m_planes[i].classifyPoint(P1) != m_planes[i].classifyPoint(P2))
		{
			++c;
		
			if(c > 1) { return true ;}
		}
	}

	return c > 1;
}

bool Frustum::intersects(const dvec3& P1, const dvec3& P2, const dvec3& P3, const dvec3& P4) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if(	m_planes[i].classifyPoint(P1) == Plane::SIDE_BACK &&
			m_planes[i].classifyPoint(P2) == Plane::SIDE_BACK &&
			m_planes[i].classifyPoint(P3) == Plane::SIDE_BACK &&
			m_planes[i].classifyPoint(P4) == Plane::SIDE_BACK) { return false ;}
	}

	return true;
}

bool Frustum::containsPoint(const dvec3 & P) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if(m_planes[i].classifyPoint(P) == Plane::SIDE_BACK) { return false ;}
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

const Plane & Frustum::getPlane(const size_t index) const { return m_planes[index] ;}


