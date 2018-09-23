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

#ifndef __TWO_WAY_PORTAL_H__
#define __TWO_WAY_PORTAL_H__

#include <tceGeom/face.h>
#include <tceGeom/portalContainer.h>

namespace tce 
{
namespace geom
{

struct TwoWayPortal : BSPFacePolylf
{

    int numOfLeafs;
    int leafOwnerArray[2];

    Plane3f & plane;

    TwoWayPortal * next;
    TwoWayPortal * prev;

    TwoWayPortal () : numOfLeafs(0), plane(*(new Plane3f)), next(NULL), prev(NULL)
    {
        leafOwnerArray[0] = -1;
        leafOwnerArray[0] = -1;
    }

    TwoWayPortal(Plane3f & PLANE, const AABB3Df & BB) : BSPFacePolylf(),
            numOfLeafs(0), plane(PLANE), next(NULL), prev(NULL)
    {
        leafOwnerArray[0] = -1;
        leafOwnerArray[0] = -1;
        makeFromPlane(PLANE, BB);
    }

    TwoWayPortal(const TwoWayPortal & portal) : numOfLeafs(0), plane(*(new Plane3f)), next(NULL), prev(NULL)
    {
        // TODO
        leafOwnerArray[0] = -1;
        leafOwnerArray[0] = -1;
    }

    TwoWayPortal & operator=(const TwoWayPortal & portal)
    {
        // TODO
        return *this;
    }

    TwoWayPortal * splitPortal(Plane3f & spliPlane)
    {
        TwoWayPortal * newPortal = new TwoWayPortal;
        if(splitPolygon(spliPlane, plane, (Polygon <vertexList < Vec3f >, int > *)newPortal))
        {

            return newPortal;

        }
        else
        {
            delete newPortal;
            return NULL;
        }
    }

    void removeFromList()
    {

        if(prev)
        {
            if(next) { prev->next = next ;}
            else { prev->next = NULL ;}
        }

        if(next)
        {
            if(prev) { next->prev = prev ;}
            else { next->prev = NULL ;}
        }

        delete this;
    }

};

typedef BSPPortalContainer <std::vector < TwoWayPortal * > > TwoWayPortalVector;
typedef BSPPortalContainer <std::list < TwoWayPortal * > > TwoWayPortalList;

}
}

#endif // __TWO_WAY_PORTAL_H__
