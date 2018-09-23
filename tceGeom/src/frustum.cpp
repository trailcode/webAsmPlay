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

#include <glm/gtc/type_ptr.hpp>
#include <tceGeom/frustum.h>

using namespace std;
using namespace tce::geom;

using glm::mat4;

Frustum::Frustum()
{

}

Frustum::Frustum(const mat4 & modelViewProjection)
{
    set(modelViewProjection);
}

void Frustum::set(const mat4 & modelViewProjection)
{
    const float * mvp = (const float *)glm::value_ptr(modelViewProjection);

    /*
     MVP[0][0] 0 ,MVP[0][1] 1 ,MVP[0][2] 2 ,MVP[0][3] 3,
     MVP[1][0] 4 ,MVP[1][1] 5 ,MVP[1][2] 6 ,MVP[1][3] 7,
     MVP[2][0] 8 ,MVP[2][1] 9 ,MVP[2][2] 10,MVP[2][3] 11,
     MVP[3][0] 12,MVP[3][1] 13,MVP[3][2] 14,MVP[3][3]) 15
     */

    //const float

    planes[0]  = Plane3d(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8],  mvp[15]-mvp[12]);
    planes[1]   = Plane3d(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8],  mvp[15]+mvp[12]);
    planes[2] = Plane3d(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9],  mvp[15]+mvp[13]);
    planes[3]    = Plane3d(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9],  mvp[15]-mvp[13]);
    planes[4]    = Plane3d(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14]);
    planes[5]   = Plane3d(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14]);

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

bool Frustum::AABBInFrustum(const AABB3Dd & AABB) const
{
    for(size_t i = 0; i < 6; ++i)
    {
        //if(AABB.classify(planes[i]) == SIDE_BACK) { return false ;}
        if(AABB.classify(planes[i]) == 1) { return false ;}
    }

    return true;
}


