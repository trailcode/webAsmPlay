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
 
#ifndef __HSR_H__
#define __HSR_H__

//#include <tceGeom/quake3/Q3BSPMap.h>
#include <tceGeom/brushConverter.h>
#include <tceGeom/brush.h>
#include <tceGeom/face.h>

namespace tce
{
	namespace geom
	{
		class BSPTree;
		
		class HiddenSurfaceRemoval {
			public:
				HiddenSurfaceRemoval();
				HiddenSurfaceRemoval(const HiddenSurfaceRemoval & hiddenSurfaceRemoval);
				~HiddenSurfaceRemoval();
				HiddenSurfaceRemoval & operator= (const HiddenSurfaceRemoval & hiddenSurfaceRemoval);
				//void setQ3Map(quake3::Q3BSPMap * map);
				void setBSPTree(BSPTree * BSPTree);
				void addBrush(MapBrushf & mapBrush);
				//void addBrush(quake3::Q3BSPBrush & brush);
				void addMesh(BSPFacePolyvfVector * mesh);
				void doHSR();
				
				BSPFacePolyvfVector * faces;
				
			private:
				void getPolys(BSPTree & BSPTree);
				BrushConverter<BSPFacePolyvfVector, BSPPolyPlaneAccess < Plane3f, BSPFacePolyvfVector::polyType> > brushConverter;
				
				std::list < BSPFacePolyvfVector * > meshes;
		};
	}
}

#endif // __HSR_H__

