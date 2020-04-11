#include <iostream>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/TrackBallInteractor.h>
#include <glm/gtx/norm.hpp> // length2
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp> // pi

using namespace std;

namespace rsmz
{
    const glm::vec3 TrackBallInteractor::X(1.f, 0.f, 0.f);
    const glm::vec3 TrackBallInteractor::Y(0.f, 1.f, 0.f);
    const glm::vec3 TrackBallInteractor::Z(0.f, 0.f, 1.f);

    TrackBallInteractor::TrackBallInteractor() :    m_CameraMotionLeftClick(ARC),
                                                    m_CameraMotionMiddleClick(ROLL),
                                                    m_CameraMotionRightClick(FIRSTPERSON),
                                                    m_CameraMotionScroll(ZOOM),
                                                    m_Height(1),
                                                    m_IsDragging(false),
                                                    m_IsLeftClick(false),
                                                    m_IsMiddleClick(false),
                                                    m_IsRightClick(false),
                                                    m_IsScrolling(false),
                                                    m_PanScale(.005f),
                                                    m_RollScale(.005f),
                                                    m_RollSum(0.f),
                                                    m_Rotation(1.f, 0, 0, 0),
                                                    m_RotationSum(1.f, 0, 0, 0),
                                                    m_Speed(1.f),
                                                    m_Width(1),
                                                    m_ZoomScale(1.0f * 0.2f),
                                                    m_ZoomSum(0.f),
                                                    m_Camera(nullptr),
													m_panAxis(PanAxis::ALL)
    {
    }

    TrackBallInteractor::~TrackBallInteractor()
    {
    }

    char TrackBallInteractor::clickQuadrant(float x, float y)
    {
        float halfw = .5f * m_Width;
        float halfh = .5f * m_Height;

        if (x > halfw) {
            // Opengl image coordinates origin is upperleft.
            if (y < halfh) {
                return 1;
            } else {
                return 4;
            }
        } else {
            if (y < halfh) {
                return 2;
            } else {
                return 3;
            }
        }
    }

    void TrackBallInteractor::computeCameraEye(glm::vec3 & eye)
    {
        //dmess("computeCameraEye");

		if(!m_Camera)
		{
			dmess("Warn no m_Camera!");

			return;
		}

        glm::vec3 orientation = m_RotationSum * Z;

        if (m_ZoomSum) {
            m_TranslateLength += m_ZoomScale * m_ZoomSum;
            m_ZoomSum = 0; // Freeze zooming after applying.
        }

        eye = (m_TranslateLength) * orientation + m_Camera->getCenterConstRef();
    }

    void TrackBallInteractor::computeCameraUp(glm::vec3 & up)
    {
        up = glm::normalize(m_RotationSum * Y);
    }

    void TrackBallInteractor::computePan(glm::vec3 & pan)
    {
        glm::vec2 click = m_ClickPoint - m_PrevClickPoint;
        glm::vec3 look = m_Camera->getEyeConstRef() - m_Camera->getCenterConstRef();
        float length = glm::length(look);
        //float length = 1.0;
        glm::vec3 right = glm::normalize(m_RotationSum * -X);
        //glm::vec3 right = glm::normalize(m_RotationSum * -Y);

        glm::vec3 up = m_Camera->getUpConstRef();
        
		switch(m_panAxis)
		{
			case PanAxis::X: up.x = 0; break;
			case PanAxis::Y: up.y = 0; break;
			case PanAxis::Z: up.z = 0; break;
		}

		up.z = 0;

        //up = glm::normalize(up);
        //pan = (up * click.y + right * click.x) * mPanScale * mSpeed * length;
        //pan = (up * click.y + right * click.x) * mPanScale * length;
        pan = (up * click.y + right * click.x) * m_PanScale * length;
        //pan = (mCamera->getUpConstRef() * click.y + right * click.x);
    }

    void TrackBallInteractor::computePointOnSphere(const glm::vec2 & point, glm::vec3 & result)
    {
        // https://www.opengl.org/wiki/Object_Mouse_Trackball
        float x = (2.f * point.x - m_Width) / m_Width;
        float y = (m_Height - 2.f * point.y) / m_Height;

        float length2 = x*x + y*y;

        if (length2 <= .5) {
            result.z = float(sqrt(1.0 - length2));
        } else {
            result.z = float(0.5 / sqrt(length2));
        }

        float norm = float(1.0 / sqrt(length2 + result.z*result.z));

        result.x = x * norm;
        result.y = y * norm;
        result.z *= norm;
    }

    void TrackBallInteractor::computeRotationBetweenVectors(
            const glm::vec3 & u, const glm::vec3 & v, glm::quat & result)
    {
        float cosTheta = glm::dot(u, v);
        glm::vec3 rotationAxis;

		float theta = acos(cosTheta);
        rotationAxis = glm::cross(u, v);

        rotationAxis = glm::normalize(rotationAxis);
        result = glm::angleAxis(theta * m_Speed, rotationAxis);
    }

    void TrackBallInteractor::drag()
    {
        if (m_PrevClickPoint == m_ClickPoint) {
            // Not moving during drag state, so skip unnecessary processing.
            return;
        }

        computePointOnSphere(m_ClickPoint, m_StopVector);
        computeRotationBetweenVectors(m_StartVector,
                                      m_StopVector,
                                      m_Rotation);
        // Reverse so scene moves with cursor and not away due to camera model.
        m_Rotation = glm::inverse(m_Rotation);

        drag(m_IsLeftClick, m_CameraMotionLeftClick);
        drag(m_IsMiddleClick, m_CameraMotionMiddleClick);
        drag(m_IsRightClick, m_CameraMotionRightClick);

        // After applying drag, reset relative start state.
        m_PrevClickPoint = m_ClickPoint;
        m_StartVector = m_StopVector;
    }

    void TrackBallInteractor::drag(bool isClicked, CameraMotionType motion)
    {
        if (!isClicked) {
            return;
        }

        switch(motion) {
            case ARC:
                dragArc();
                break;
            case FIRSTPERSON:
                dragFirstPerson();
                break;
            case PAN:
                dragPan();
                break;
            case ROLL:
                rollCamera();
                break;
            case ZOOM:
                dragZoom();
                break;
            default: break;
        }
    }

    void TrackBallInteractor::dragArc()
    {
        m_RotationSum *= m_Rotation; // Accumulate quaternions.

        updateCameraEyeUp(true, true);
    }

    void TrackBallInteractor::dragFirstPerson()
    {
        glm::vec3 pan;
        computePan(pan);
        m_Camera->setCenter(pan + m_Camera->getCenterConstRef());
        m_Camera->update();
        freezeTransform();
    }

    void TrackBallInteractor::dragPan()
    {
        glm::vec3 pan;
        computePan(pan);
        m_Camera->setCenter(pan + m_Camera->getCenterConstRef());
        m_Camera->setEye(pan + m_Camera->getEyeConstRef());
        m_Camera->update();
        freezeTransform();
    }

    void TrackBallInteractor::dragZoom()
    {
        glm::vec2 dir = m_ClickPoint - m_PrevClickPoint;
        float ax = fabs(dir.x);
        float ay = fabs(dir.y);

        if (ay >= ax) {
            setScrollDirection(dir.y <= 0);
        } else {
            setScrollDirection(dir.x <= 0);
        }

        updateCameraEyeUp(true, false);
    }

    void TrackBallInteractor::freezeTransform()
    {
        if (m_Camera) {
            // Opengl is ZYX order.
            // Flip orientation to rotate scene with sticky cursor.
            m_RotationSum = glm::inverse(glm::quat(m_Camera->getMatrixConstRef()));
            m_TranslateLength = glm::length(m_Camera->getEyeConstRef()-m_Camera->getCenterConstRef());
        }
    }

    Camera* TrackBallInteractor::getCamera()
    {
        if(!m_Camera) { setCamera(new Camera()) ;}

        return m_Camera;
    }

    CameraMotionType TrackBallInteractor::getMotionLeftClick()
    {
        return m_CameraMotionLeftClick;
    }

    CameraMotionType TrackBallInteractor::getMotionMiddleClick()
    {
        return m_CameraMotionMiddleClick;
    }

    CameraMotionType TrackBallInteractor::getMotionRightClick()
    {
        return m_CameraMotionRightClick;
    }

    CameraMotionType TrackBallInteractor::getMotionScroll()
    {
        return m_CameraMotionScroll;
    }

    void TrackBallInteractor::rollCamera()
    {
        glm::vec2 delta = m_ClickPoint - m_PrevClickPoint;
        char quad = clickQuadrant(m_ClickPoint.x, m_ClickPoint.y);
        switch (quad) {
            case 1:
                delta.y = -delta.y;
                delta.x = -delta.x;
                break;
            case 2:
                delta.x = -delta.x;
                break;
            case 3:
                break;
            case 4:
                delta.y = -delta.y;
            default:
                break;
        }

        glm::vec3 axis = glm::normalize(m_Camera->getCenterConstRef() - m_Camera->getEyeConstRef());
        float angle = m_RollScale * m_Speed * (delta.x + delta.y + m_RollSum);
        glm::quat rot = glm::angleAxis(angle, axis);
        m_Camera->setUp(rot * m_Camera->getUpConstRef());
        m_Camera->update();
        freezeTransform();
        m_RollSum = 0;
    }

    void TrackBallInteractor::scroll()
    {
        switch(m_CameraMotionScroll) {
            case ROLL:
                rollCamera();
                break;
            case ZOOM:
                updateCameraEyeUp(true, false);
                break;
            default: break;
        }
    }

    void TrackBallInteractor::setCamera(Camera *c)
    {
        m_Camera = c;
        freezeTransform();
    }

    void TrackBallInteractor::setClickPoint(double x, double y)
    {
        m_PrevClickPoint = m_ClickPoint;
        m_ClickPoint.x = float(x);
        m_ClickPoint.y = float(y);
    }

    void TrackBallInteractor::setLeftClicked(bool value)
    {
        m_IsLeftClick = value;
    }

    void TrackBallInteractor::setMiddleClicked(bool value)
    {
        m_IsMiddleClick = value;
    }

    void TrackBallInteractor::setMotionLeftClick(CameraMotionType motion)
    {
        m_CameraMotionLeftClick = motion;
    }

    void TrackBallInteractor::setMotionMiddleClick(CameraMotionType motion)
    {
        m_CameraMotionMiddleClick = motion;
    }

    void TrackBallInteractor::setMotionRightClick(CameraMotionType motion)
    {
        m_CameraMotionRightClick = motion;
    }

    void TrackBallInteractor::setMotionScroll(CameraMotionType motion)
    {
        m_CameraMotionScroll = motion;
    }

    void TrackBallInteractor::setRightClicked(bool value)
    {
        m_IsRightClick = value;
    }

    void TrackBallInteractor::setScreenSize(float width, float height)
    {
        if (width > 1 && height > 1) {
            m_Width = width;
            m_Height = height;
        }
    }

    void TrackBallInteractor::setScrollDirection(bool up)
    {
        m_IsScrolling = true;
        float inc = m_Speed * (up ? -1.f : 1.f);
        m_ZoomSum += inc;
        m_RollSum += inc;
    }

    void TrackBallInteractor::setSpeed(float s)
    {
        m_Speed = s;
    }

    void TrackBallInteractor::update()
    {
        const bool isClick = m_IsLeftClick || m_IsMiddleClick || m_IsRightClick;

        if (! m_IsDragging)
        {
            if (isClick)
            {
                m_IsDragging = true;
                computePointOnSphere(m_ClickPoint, m_StartVector);
            } else if (m_IsScrolling) {
                scroll();
                m_IsScrolling = false;
            }
        } else
        {
            if (isClick)
            {
                drag();
            } else
            {
                m_IsDragging = false;
            }
        }
    }

    void TrackBallInteractor::updateCameraEyeUp(bool eye, bool up)
    {
		if(!m_Camera) { return ;}

        if (eye) {
            glm::vec3 eye;
            computeCameraEye(eye);
            m_Camera->setEye(eye);
        }
        if (up) {
            glm::vec3 up;
            computeCameraUp(up);
            m_Camera->setUp(up);
        }
        m_Camera->update();
    }

    float TrackBallInteractor::setZoomScale	(const float scale) { return m_ZoomScale = scale ;}
	float TrackBallInteractor::setPanScale	(const float scale) { return m_PanScale  = scale ;}
	float TrackBallInteractor::setRollScale	(const float scale) { return m_RollScale = scale ;}

	void TrackBallInteractor::setPanAxis(const PanAxis panAxis) { m_panAxis = panAxis ;}


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
