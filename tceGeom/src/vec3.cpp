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

#include <tceGeom/vec3.h>

using namespace tce::geom;

// ----------------------------------------------------------------------------
// names for frequently used vector constants


template<> const Vec3f Vec3f::zero    (0, 0, 0);
template<> const Vec3f Vec3f::up      (0, 1, 0);
template<> const Vec3f Vec3f::forward (0, 0, 1);

// XXX  This should be unified with LocalSpace::rightHanded, but I don't want
// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there
// XXX  should be a tiny chirality.h header to define a const?  That could
// XXX  then be included by both Vec3.h and LocalSpace.h

template<> const Vec3f Vec3f::side    (-1, 0, 0);

// ----------------------------------------------------------------------------
// names for frequently used vector constants


template<> const Vec3d Vec3d::zero    (0, 0, 0);
template<> const Vec3d Vec3d::up      (0, 1, 0);
template<> const Vec3d Vec3d::forward (0, 0, 1);

// XXX  This should be unified with LocalSpace::rightHanded, but I don't want
// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there
// XXX  should be a tiny chirality.h header to define a const?  That could
// XXX  then be included by both Vec3.h and LocalSpace.h

template<> const Vec3d Vec3d::side    (-1, 0, 0);

// ----------------------------------------------------------------------------
// names for frequently used vector constants


template<> const Vec3i Vec3i::zero    (0, 0, 0);
template<> const Vec3i Vec3i::up      (0, 1, 0);
template<> const Vec3i Vec3i::forward (0, 0, 1);

// XXX  This should be unified with LocalSpace::rightHanded, but I don't want
// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there
// XXX  should be a tiny chirality.h header to define a const?  That could
// XXX  then be included by both Vec3.h and LocalSpace.h

template<> const Vec3i Vec3i::side    (-1, 0, 0);
