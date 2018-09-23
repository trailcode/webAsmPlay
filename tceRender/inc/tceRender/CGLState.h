#ifndef __CGLSTATE_H_
#define __CGLSTATE_H_

#include <tceGeom/Q3BSPMap.h>

namespace tce
{

	class CGLState {
		public:
			// Following functions change manually an opengl state
			static void setDepthFunc(const geom::quake3::Q3BSPShaderLayer & layer);
			static void setDepthWrite(const geom::quake3::Q3BSPShaderLayer & layer);
			static void setBlendFunc(const geom::quake3::Q3BSPShaderLayer & layer);
			static void setCullFunc(const geom::quake3::Q3BSPShader & shader);
			static void setAlphaFunc(const geom::quake3::Q3BSPShaderLayer & layer);
			static void setPolygonMode(int val);
			static void setDefaultState();
	};

}

#endif // __CGLSTATE_H_
