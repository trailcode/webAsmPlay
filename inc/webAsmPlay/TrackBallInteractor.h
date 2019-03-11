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

#include <webAsmPlay/Camera.h>
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
        void update();

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
        void updateCameraEyeUp(bool eye, bool up);

    private:
        Camera *mCamera;
        CameraMotionType mCameraMotionLeftClick;
        CameraMotionType mCameraMotionMiddleClick;
        CameraMotionType mCameraMotionRightClick;
        CameraMotionType mCameraMotionScroll;
        glm::vec2 mClickPoint;
        float mHeight;
        bool mIsDragging;
        bool mIsLeftClick;
        bool mIsMiddleClick;
        bool mIsRightClick;
        bool mIsScrolling;
        float mPanScale;
        glm::vec2 mPrevClickPoint;
        float mRollScale;
        float mRollSum;
        glm::quat mRotation;
        glm::quat mRotationSum;
        float mSpeed;
        glm::vec3 mStartVector;
        glm::vec3 mStopVector;
        float mTranslateLength;
        float mWidth;
        float mZoomSum;
        float mZoomScale;

    }; // end class TrackBallInteractor

} // end namespace rsmz

