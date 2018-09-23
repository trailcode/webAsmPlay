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

#ifndef __T_BRUSH_H__
#define __T_BRUSH_H__

#include <string>
#include <list>
#include <tceGeom/vec3.h>

namespace tce
{
	namespace geom
	{

		template <typename sideContainer, typename textureType>
		struct Brush
		{

			sideContainer  brushSides;
			textureType    texture;
			
			typedef typename sideContainer::iterator       iterator;
			typedef typename sideContainer::const_iterator const_iterator;
		};

		template <typename pointType>
		struct Triangle
		{
			pointType points[3];
		};

		typedef Triangle < Vec3f > Trianglef;
		typedef Triangle < Vec3d > Triangled;
		typedef Triangle < Vec3i > Trianglei;

		template <typename planeType, typename textureType>
		struct BrushSide
		{
			planeType   plane;
			textureType texture;
			
			BrushSide(planeType P, textureType T) : plane(P), texture(T) {; };
		};

		typedef BrushSide <Trianglef, std::string> MapBrushSidef;

		typedef Brush <std::list < MapBrushSidef >, char> MapBrushf;
	}
}

#endif // __T_BRUSH_H__
