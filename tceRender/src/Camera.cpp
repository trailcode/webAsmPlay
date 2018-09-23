#include <tceRender/Camera.h>

using namespace tce;
using namespace tce::geom;

Camera::Camera()
{
}

Camera::~Camera()
{
}

Vec3f
Camera::getPosition() const
{
	return position;
}

Vec3f
Camera::setPosition(const Vec3f & pos)
{
	position = pos;
	
	return position;
}