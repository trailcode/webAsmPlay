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
#pragma once

#include <glm/mat4x4.hpp>
#include <webAsmPlay/geom/AABB3D.h>

class Frustum
{
public:

    enum {
        RIGHT_CLIP_PLANE = 0,
        LEFT_CLIP_PLANE,
        BOTTOM_CLIP_PLANE,
        TOP_CLIP_PLANE,
        FAR_CLIP_PLANE,
        NEAR_CLIP_PLANE,
    };

    Frustum();

	Frustum(const glm::dmat4 & modelViewProjection);

    void set(const glm::dmat4 & modelViewProjection);

	bool AABBInFrustum(const AABB3D & AABB) const;

	const Plane & getPlane(const size_t index) const;

	bool containsPoint(const glm::dvec3 & P) const;

	bool containsA_Point(const glm::dvec3 & P1, const glm::dvec3 & P2, const glm::dvec3 & P3, const glm::dvec3 & P4) const;

protected:

    Plane planes[6];

private:

};
