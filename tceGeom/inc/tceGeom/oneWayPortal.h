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

#ifndef __ONE_WAY_PORTAL_H__
#define __ONE_WAY_PORTAL_H__

#include <tceGeom/plane.h>
#include <tceGeom/twoWayPortal.h>
#include <tceGeom/portalContainer.h>

namespace tce
{
	namespace geom
	{

		struct BSPLeaf;

		struct OneWayPortal
		{
			vertexList < Vec3f > & verts;
			Plane3f plane;
			BSPLeaf * neighborLeaf;

			OneWayPortal(TwoWayPortal * portal, bool inverPlane = false) : verts(portal->verts), plane(Plane3f()), neighborLeaf(NULL)
			{

				if(inverPlane) { plane.setInverted(portal->plane) ;}

				else { this->plane = portal->plane ;}
			}

			OneWayPortal(const OneWayPortal & portal) : verts(portal.verts), plane(portal.plane), neighborLeaf(portal.neighborLeaf)
			{
			}

			OneWayPortal & operator=(const OneWayPortal & portal)
			{
				verts = portal.verts;
				plane = portal.plane;
				neighborLeaf = portal.neighborLeaf;
				return *this;
			}
		};

		typedef BSPPortalContainer <std::vector < OneWayPortal * > > OneWayPortalVector;
		typedef BSPPortalContainer <std::list < OneWayPortal * > > OneWayPortalList;

	}
}

#endif // __ONE_WAY_PORTAL_H__
