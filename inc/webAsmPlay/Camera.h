#ifndef RSMZ_CAMERA_H
#define RSMZ_CAMERA_H

#include <vector>
#include <glm/glm.hpp>

namespace rsmz
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        const glm::mat4 & getMatrixConstRef();
        glm::mat4 getMatrix();
        const float* getMatrixFlatPtr();
        std::vector<float> getMatrixFlat();
        const glm::vec3 & getCenterConstRef();
        const glm::vec3 & getEyeConstRef();
        const glm::vec3 & getUpConstRef();
        glm::vec3 getCenter();
        glm::vec3 getEye();
        glm::vec3 getUp();
        void reset();
        void setMatrix(const glm::mat4 & MVP);
        void setCenter(float x, float y, float z);
        void setCenter(const glm::vec3 & c);
        void setEye(float x, float y, float z);
        void setEye(const glm::vec3 & e);
        void setUp(float x, float y, float z);
        void setUp(const glm::vec3 & u);
        void update();

    private:
        glm::vec3 mCenter;
        glm::vec3 mEye;
        glm::mat4 mMatrix;
        glm::vec3 mUp;
    };

} // end namespace rsmz

#endif // RSMZ_CAMERA_H

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
