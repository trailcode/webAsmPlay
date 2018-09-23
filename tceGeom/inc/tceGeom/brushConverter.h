#ifndef __BRUSH_CONVERTER_H_
#define __BRUSH_CONVERTER_H_

#include <list>
#include <tceGeom/brush.h>
#include <tceGeom/polygon.h>
#include <tceGeom/BSPTree.h>
//#include <tceGeom/quake3/Q3BSPMap.h>

namespace tce
{
	namespace geom
	{
		template <typename planeType, typename polyType>
		struct DefaultPolyPlaneAccess
		{
			const planeType * getPlane(const polyType & poly) const { return &(poly.plane) ;}
		};

		template <typename planeType, typename polyType>
		struct BSPPolyPlaneAccess
		{
			BSPPolyPlaneAccess() : tree(NULL) { ;}

			BSPPolyPlaneAccess(BSPTree * _tree) : tree(_tree) { ;}

			const planeType * getPlane(polyType & poly) const { return tree->addPlane(poly) ;}

			BSPTree * tree;

		};

		template <	typename polyContainer,
					typename polyPlaneAccess = DefaultPolyPlaneAccess< typename polyContainer::polyType::planeTypeType, typename polyContainer::polyType> >
		class BrushConverter
		{
		public:

			typedef typename polyContainer::polyType       polyType;
			typedef typename polyContainer::const_iterator polyContainer_const_iterator;
			typedef typename polyContainer::iterator       polyContainer_iterator;
			
			typedef typename polyType::iterator            polyVertexIterator;
			typedef typename polyType::const_iterator      polyVertexConstIterator;
			typedef typename polyType::vertexContainerType polyTypeVertexContainer;

			typedef typename polyContainer::polyType::planeTypeType planeType;

			BrushConverter();
			
			BrushConverter(const BrushConverter & converter);
			
			BrushConverter(const polyPlaneAccess & _planeAccess);
			
			~BrushConverter();
			
			BrushConverter & operator= (const BrushConverter & converter);
			
			//void setQ3Map(quake3::Q3BSPMap * map);
			
			void setPlaneAccess(const polyPlaneAccess & planeAccess);
			
			//polyContainer * convert(quake3::Q3BSPBrush &brush, polyContainer & polygons);
			
			polyContainer * convert(MapBrushf &brush, polyContainer & polygons);
			
			//polyContainer * convert(quake3::Q3BSPBrush &brush);
			
			polyContainer * convert(MapBrushf &brush);

		private:
			
			void expandBrush(planefList &brushSides, const Vec3f & AABB);
			
			void addBevels(planefList & planes, polyContainer & polys);
			
			bool tryAddBevelPlane(planeType & plane, planefList & planes, const polyContainer & polys);
			
			void convert(planefList &brushSides, polyContainer & polygons);
			
			void windingClip(const Plane3f & plane, polyType & winding);
			
			//quake3::Q3BSPMap * map;
			
			polyPlaneAccess planeAccess;
		};

		//typedef BrushConverter<polyldList> BrushConverter_ldl;
	}
}
#endif // __BRUSH_CONVERTER_H_
