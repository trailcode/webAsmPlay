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

Modified by Matthew Tang (trailcode@gmail.com)
*/
#pragma once

#include <webAsmPlay/canvas/Camera.h>
#include <glm/gtc/quaternion.hpp>

namespace rsmz
{
    typedef enum CameraMotionType {
        NONE = 0,
        ARC,
        FIRSTPERSON,
        PAN,
        ROLL,
        ZOOM,
    } CameraMotionType;

	typedef enum PanAxis
	{
		ALL = 0,
		X,
		Y,
		Z
	} PanAxis;

    class TrackBallInteractor
    {
    public:

        static const glm::vec3 X, Y, Z;

        TrackBallInteractor();
        ~TrackBallInteractor();

        void computePointOnSphere(const glm::vec2 & point,
                                  glm::vec3 & result);
        void computeRotationBetweenVectors(const glm::vec3 & start,
                                           const glm::vec3 & stop,
                                           glm::quat & result);
        Camera* getCamera();
        CameraMotionType getMotionLeftClick();
        CameraMotionType getMotionMiddleClick();
        CameraMotionType getMotionRightClick();
        CameraMotionType getMotionScroll();
        void setScrollDirection(bool up);
        void setCamera(Camera *c);
        void setClickPoint(double x, double y);
        void setLeftClicked(bool value);
        void setMiddleClicked(bool value);
        void setMotionLeftClick(CameraMotionType motion);
        void setMotionMiddleClick(CameraMotionType motion);
        void setMotionRightClick(CameraMotionType motion);
        void setMotionScroll(CameraMotionType motion);
        void setRightClicked(bool value);
        void setScreenSize(float width, float height);
        void setSpeed(float s);
        float setZoomScale(const float scale);
		float setPanScale(const float scale);
		float setRollScale(const float scale);
        void update();
		void updateCameraEyeUp(bool eye, bool up);
		void setPanAxis(const PanAxis panAxis);

    protected:

        char clickQuadrant(float x, float y);
        void computeCameraEye(glm::vec3 & eye);
        void computeCameraUp(glm::vec3 & up);
        void computePan(glm::vec3 & pan);
        void drag();
        void drag(bool isClicked, CameraMotionType motion);
        void dragArc();
        // Change eye position and up direction while keeping center point static.
        void dragArcCamera();
        void dragFirstPerson();
        void dragZoom();
        // Simulate zoom by moving camera along viewing eye direction.
        void dragZoomCamera();
        // Move camera focal center position with static up and eye direction.
        void dragPan();
        // Roll about eye direction.
        void rollCamera();
        void freezeTransform();
        void scroll();
        
    private:
        Camera *m_Camera;
        CameraMotionType m_CameraMotionLeftClick;
        CameraMotionType m_CameraMotionMiddleClick;
        CameraMotionType m_CameraMotionRightClick;
        CameraMotionType m_CameraMotionScroll;
        glm::vec2 m_ClickPoint;
        float m_Height;
        bool m_IsDragging;
        bool m_IsLeftClick;
        bool m_IsMiddleClick;
        bool m_IsRightClick;
        bool m_IsScrolling;
        float m_PanScale;
        glm::vec2 m_PrevClickPoint;
        float m_RollScale;
        float m_RollSum;
        glm::quat m_Rotation;
        glm::quat m_RotationSum;
        float m_Speed;
        glm::vec3 m_StartVector;
        glm::vec3 m_StopVector;
        float m_TranslateLength;
        float m_Width;
        float m_ZoomSum;
        float m_ZoomScale;
		PanAxis m_panAxis;

    }; // end class TrackBallInteractor

} // end namespace rsmz

