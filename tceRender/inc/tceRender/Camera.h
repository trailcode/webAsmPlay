#ifndef CAMERA_H_
#define CAMERA_H_

#include <tceGeom/vec3.h>

namespace tce
{
	class Camera
	{
	public:
		Camera();
		virtual ~Camera();
		
		geom::Vec3f getPosition() const;
		geom::Vec3f setPosition(const geom::Vec3f & pos);
		
	private:
		
		geom::Vec3f position;
	};
}

#endif /*CAMERA_H_*/
