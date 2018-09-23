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

#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include <glm/glm.hpp>
#include <tceGeom/vec3.h>
#include <tceGeom/AABB3D.h>
#include <tceGeom/plane.h>

namespace tce
{
	namespace geom
	{
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

			Frustum(const glm::mat4 & modelViewProjection);

            void set(const glm::mat4 & modelViewProjection);

			bool AABBInFrustum(const AABB3Dd & AABB) const;

		protected:

            Plane3d planes[6];

		private:

		};
	}
}

#endif	// __FRUSTUM_H__

