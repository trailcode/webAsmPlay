#ifndef Q3BSPBEZIERPATCH_H_
#define Q3BSPBEZIERPATCH_H_

#include <tceGeom/vec3.h>
#include <tceGeom/vec2.h>

namespace tce
{
	namespace geom
	{
		namespace quake3
		{
			struct Q3BSPMap;
		}
		
		class Q3BSPBezierPatch
		{
		public:
			Q3BSPBezierPatch();
			Q3BSPBezierPatch(const Q3BSPBezierPatch & patch);
		    virtual ~Q3BSPBezierPatch();
		    Q3BSPBezierPatch & operator=(const Q3BSPBezierPatch & patch);
		    // CPatch interface
		    void CreatePatch(int iFaceIndex);
		
		    quake3::Q3BSPMap *map;
		    
		protected:
		    // CPatch variables
		
		    // TODO: add member variables...
		private:
			
		    Vec2f *QuadraticBezierPatchSubdivide1(Vec2f G[][3], int iLevel);
		
		    Vec3f* QuadraticBezierPatchSubdivide2(Vec3f G[][3], int iLevel);
		
		    void QuadraticBezierSubdivide1(Vec2f G[], int iLevel);
		
		    void QuadraticBezierSubdivide2(Vec3f G[], int iLevel);
		};
	}
}

#endif /*Q3BSPBEZIERPATCH_H_*/
