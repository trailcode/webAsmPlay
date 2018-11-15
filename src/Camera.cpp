#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Debug.h>
#include <tceGeom/vec3.h>

using namespace std;

namespace rsmz
{

    Camera::Camera()
    {
        reset();
    }

    Camera::~Camera()
    {
    }

    const glm::vec3 & Camera::getCenterConstRef() const
    {
        return mCenter;
    }

    glm::vec3 Camera::getCenter() const
    {
        return mCenter;
    }

    const glm::vec3 & Camera::getEyeConstRef() const
    {
        return mEye;
    }

    glm::vec3 Camera::getEye() const
    {
        return mEye;
    }

    const glm::mat4 & Camera::getMatrixConstRef() const
    {
        return mMatrix;
    }

    glm::mat4 Camera::getMatrix() const
    {
        return mMatrix;
    }

    void Camera::setMatrix(const glm::mat4 & MVP)
    {
        cout << "Does not seem to work" << endl;

        mMatrix = MVP;
    }

    const float* Camera::getMatrixFlatPtr() const
    {
        return glm::value_ptr(mMatrix);
    }

    vector<float> Camera::getMatrixFlat() const
    {
        vector<float> ret(16);

        memcpy(&ret[0], glm::value_ptr(mMatrix), sizeof(float) * 16);

        return ret;
    }

    const glm::vec3 & Camera::getUpConstRef() const
    {
        return mUp;
    }

    glm::vec3 Camera::getUp() const
    {
        return mUp;
    }

    void Camera::reset()
    {
        dmess("Camera::reset");

        mEye.x = 0.f;
        mEye.y = 0.f;
        mEye.z = 1.f;
        mCenter.x = 0.f;
        mCenter.y = 0.f;
        mCenter.z = 0.f;
        mUp.x = 0.f;
        mUp.y = 1.f;
        mUp.z = 0.f;

        update();
    }

    void Camera::setEye(float x, float y, float z)
    {
        mEye.x = x;
        mEye.y = y;
        mEye.z = z;
    }

    void Camera::setEye(const glm::vec3 & e)
    {
        mEye = e;
    }

    void Camera::setCenter(float x, float y, float z)
    {
        mCenter.x = x;
        mCenter.y = y;
        mCenter.z = z;

        mCenter.z = 0; // TODO This is a hack
    }

    void Camera::setCenter(const glm::vec3 & c)
    {
        mCenter = c;

        mCenter.z = 0; // TODO This is a hack
    }

    void Camera::setUp(float x, float y, float z)
    {
        mUp.x = x;
        mUp.y = y;
        mUp.z = z;
    }

    void Camera::setUp(const glm::vec3 & u)
    {
        mUp = u;
    }

    void Camera::update()
    {
        //dmess("Camera::update mEye: " << mEye << " mCenter: " << mCenter << " mUp: " << mUp << " " << glm::length(mUp));

        mMatrix = glm::lookAt(mEye, mCenter, mUp);
    }

} // end namespace rsmz

/*
    LICENSE BEGIN

    trackball - A 3D view interactor for C++ programs.
    Copyright (C) 2016  Remik Ziemlinski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    LICENSE END
*/
