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

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/Debug.h>

using namespace glm;
using namespace std;
using namespace rsmz;

Camera::Camera() { reset() ;}

const vec3 & Camera::getCenterConstRef() const { return m_center ;}

vec3 Camera::getCenter() const { return m_center ;}

const vec3 & Camera::getEyeConstRef() const { return m_eye ;}

vec3 Camera::getEye() const { return m_eye ;}

const mat4 & Camera::getMatrixConstRef() const { return m_matrix ;}

mat4 Camera::getMatrix() const { return m_matrix ;}

const vec3 & Camera::getUpConstRef() const { return m_up ;}

vec3 Camera::getUp() const { return m_up ;}

void Camera::setMatrix(const mat4 & MVP)
{
    dmess("Does not seem to work");

    m_matrix = MVP;
}

const float* Camera::getMatrixFlatPtr() const
{
    return value_ptr(m_matrix);
}

vector<float> Camera::getMatrixFlat() const
{
    vector<float> ret(16);

    memcpy(&ret[0], value_ptr(m_matrix), sizeof(float) * 16);

    return ret;
}

void Camera::reset()
{
    dmess("Camera::reset");

    m_eye.x = 0.f;
    m_eye.y = 0.f;
    //m_eye.z = 1.f;
	m_eye.z = -1.f;
    m_center.x = 0.f;
    m_center.y = 0.f;
    m_center.z = 0.f;
    m_up.x = 0.f;
    m_up.y = 1.f;
    m_up.z = 0.f;

    update();
}

void Camera::setEye(float x, float y, float z)
{
    m_eye.x = x;
    m_eye.y = y;
    m_eye.z = z;
}

void Camera::setEye(const vec3 & e)
{
    m_eye = e;
}

void Camera::setCenter(float x, float y, float z)
{
    m_center.x = x;
    m_center.y = y;
    m_center.z = z;

    //m_center.z = 0; // TODO This is a hack
}

void Camera::setCenter(const vec3 & c)
{
    m_center = c;

    m_center.z = 0; // TODO This is a hack
}

void Camera::setUp(float x, float y, float z)
{
    m_up.x = x;
    m_up.y = y;
    m_up.z = z;
}

void Camera::setUp(const vec3 & u) { m_up = u ;}

void Camera::update() { m_matrix = lookAt(m_eye, m_center, m_up) ;}
