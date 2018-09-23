#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "debug.h"
#include "TrackBallInteractor.h"
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

    TrackBallInteractor::TrackBallInteractor() :
            mCameraMotionLeftClick(ARC),
            mCameraMotionMiddleClick(ROLL),
            mCameraMotionRightClick(FIRSTPERSON),
            mCameraMotionScroll(ZOOM),
            mHeight(1),
            mIsDragging(false),
            mIsLeftClick(false),
            mIsMiddleClick(false),
            mIsRightClick(false),
            mIsScrolling(false),
            mPanScale(.005f),
            mRollScale(.005f),
            mRollSum(0.f),
            mRotation(1.f, 0, 0, 0),
            mRotationSum(1.f, 0, 0, 0),
            mSpeed(1.f),
            mWidth(1),
            mZoomScale(.1f * 0.2f),
            mZoomSum(0.f),
            mCamera(NULL)
    {
    }

    TrackBallInteractor::~TrackBallInteractor()
    {
    }

    char TrackBallInteractor::clickQuadrant(float x, float y)
    {
        float halfw = .5 * mWidth;
        float halfh = .5 * mHeight;

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

        glm::vec3 orientation = mRotationSum * Z;

        if (mZoomSum) {
            mTranslateLength += mZoomScale * mZoomSum;
            mZoomSum = 0; // Freeze zooming after applying.
        }

        eye = (mTranslateLength) * orientation + mCamera->getCenterConstRef();
    }

    void TrackBallInteractor::computeCameraUp(glm::vec3 & up)
    {
        up = glm::normalize(mRotationSum * Y);
    }

    void TrackBallInteractor::computePan(glm::vec3 & pan)
    {
        glm::vec2 click = mClickPoint - mPrevClickPoint;
        glm::vec3 look = mCamera->getEyeConstRef() - mCamera->getCenterConstRef();
        float length = glm::length(look);
        //float length = 1.0;
        glm::vec3 right = glm::normalize(mRotationSum * -X);
        //glm::vec3 right = glm::normalize(-X);
        glm::vec3 up = mCamera->getUpConstRef();
        up.z = 0;
        //up = glm::normalize(up);
        //pan = (up * click.y + right * click.x) * mPanScale * mSpeed * length;
        //pan = (up * click.y + right * click.x) * mPanScale * length;
        pan = (up * click.y + right * click.x) * mPanScale * length;
        //pan = (mCamera->getUpConstRef() * click.y + right * click.x);
    }

    void TrackBallInteractor::computePointOnSphere(const glm::vec2 & point, glm::vec3 & result)
    {
        // https://www.opengl.org/wiki/Object_Mouse_Trackball
        float x = (2.f * point.x - mWidth) / mWidth;
        float y = (mHeight - 2.f * point.y) / mHeight;

        float length2 = x*x + y*y;

        if (length2 <= .5) {
            result.z = sqrt(1.0 - length2);
        } else {
            result.z = 0.5 / sqrt(length2);
        }

        float norm = 1.0 / sqrt(length2 + result.z*result.z);

        result.x = x * norm;
        result.y = y * norm;
        result.z *= norm;
    }

    void TrackBallInteractor::computeRotationBetweenVectors(
            const glm::vec3 & u, const glm::vec3 & v, glm::quat & result)
    {
        float cosTheta = glm::dot(u, v);
        glm::vec3 rotationAxis;
        static const float EPSILON = 1.0e-5f;

        if (cosTheta < -1.0f + EPSILON){
            // Parallel and opposite directions.
            rotationAxis = glm::cross(glm::vec3(0.f, 0.f, 1.f), u);

            if (glm::length2(rotationAxis) < 0.01 ) {
                // Still parallel, retry.
                rotationAxis = glm::cross(glm::vec3(1.f, 0.f, 0.f), u);
            }

            rotationAxis = glm::normalize(rotationAxis);
            result = glm::angleAxis(180.0f, rotationAxis);
        } else if (cosTheta > 1.0f - EPSILON) {
            // Parallel and same direction.
            result = glm::quat(1, 0, 0, 0);
            return;
        } else {
            float theta = acos(cosTheta);
            rotationAxis = glm::cross(u, v);

            rotationAxis = glm::normalize(rotationAxis);
            result = glm::angleAxis(theta * mSpeed, rotationAxis);
        }
    }

    void TrackBallInteractor::drag()
    {
        if (mPrevClickPoint == mClickPoint) {
            // Not moving during drag state, so skip unnecessary processing.
            return;
        }

        computePointOnSphere(mClickPoint, mStopVector);
        computeRotationBetweenVectors(mStartVector,
                                      mStopVector,
                                      mRotation);
        // Reverse so scene moves with cursor and not away due to camera model.
        mRotation = glm::inverse(mRotation);

        drag(mIsLeftClick, mCameraMotionLeftClick);
        drag(mIsMiddleClick, mCameraMotionMiddleClick);
        drag(mIsRightClick, mCameraMotionRightClick);

        // After applying drag, reset relative start state.
        mPrevClickPoint = mClickPoint;
        mStartVector = mStopVector;
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
        mRotationSum *= mRotation; // Accumulate quaternions.

        updateCameraEyeUp(true, true);
    }

    void TrackBallInteractor::dragFirstPerson()
    {
        glm::vec3 pan;
        computePan(pan);
        mCamera->setCenter(pan + mCamera->getCenterConstRef());
        mCamera->update();
        freezeTransform();
    }

    void TrackBallInteractor::dragPan()
    {
        glm::vec3 pan;
        computePan(pan);
        mCamera->setCenter(pan + mCamera->getCenterConstRef());
        mCamera->setEye(pan + mCamera->getEyeConstRef());
        mCamera->update();
        freezeTransform();
    }

    void TrackBallInteractor::dragZoom()
    {
        cout << "dragZoom " << endl;
        glm::vec2 dir = mClickPoint - mPrevClickPoint;
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
        if (mCamera) {
            // Opengl is ZYX order.
            // Flip orientation to rotate scene with sticky cursor.
            mRotationSum = glm::inverse(glm::quat(mCamera->getMatrixConstRef()));
            mTranslateLength = glm::length(mCamera->getEyeConstRef()-mCamera->getCenterConstRef());
        }
    }

    Camera* TrackBallInteractor::getCamera()
    {
        if(!mCamera) { setCamera(new Camera()) ;}

        return mCamera;
    }

    CameraMotionType TrackBallInteractor::getMotionLeftClick()
    {
        return mCameraMotionLeftClick;
    }

    CameraMotionType TrackBallInteractor::getMotionMiddleClick()
    {
        return mCameraMotionMiddleClick;
    }

    CameraMotionType TrackBallInteractor::getMotionRightClick()
    {
        return mCameraMotionRightClick;
    }

    CameraMotionType TrackBallInteractor::getMotionScroll()
    {
        return mCameraMotionScroll;
    }

    void TrackBallInteractor::rollCamera()
    {
        glm::vec2 delta = mClickPoint - mPrevClickPoint;
        char quad = clickQuadrant(mClickPoint.x, mClickPoint.y);
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

        glm::vec3 axis = glm::normalize(mCamera->getCenterConstRef() - mCamera->getEyeConstRef());
        float angle = mRollScale * mSpeed * (delta.x + delta.y + mRollSum);
        glm::quat rot = glm::angleAxis(angle, axis);
        mCamera->setUp(rot * mCamera->getUpConstRef());
        mCamera->update();
        freezeTransform();
        mRollSum = 0;
    }

    void TrackBallInteractor::scroll()
    {
        switch(mCameraMotionScroll) {
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
        mCamera = c;
        freezeTransform();
    }

    void TrackBallInteractor::setClickPoint(double x, double y)
    {
        mPrevClickPoint = mClickPoint;
        mClickPoint.x = x;
        mClickPoint.y = y;
    }

    void TrackBallInteractor::setLeftClicked(bool value)
    {
        mIsLeftClick = value;
    }

    void TrackBallInteractor::setMiddleClicked(bool value)
    {
        mIsMiddleClick = value;
    }

    void TrackBallInteractor::setMotionLeftClick(CameraMotionType motion)
    {
        mCameraMotionLeftClick = motion;
    }

    void TrackBallInteractor::setMotionMiddleClick(CameraMotionType motion)
    {
        mCameraMotionMiddleClick = motion;
    }

    void TrackBallInteractor::setMotionRightClick(CameraMotionType motion)
    {
        mCameraMotionRightClick = motion;
    }

    void TrackBallInteractor::setMotionScroll(CameraMotionType motion)
    {
        mCameraMotionScroll = motion;
    }

    void TrackBallInteractor::setRightClicked(bool value)
    {
        mIsRightClick = value;
    }

    void TrackBallInteractor::setScreenSize(float width, float height)
    {
        if (width > 1 && height > 1) {
            mWidth = width;
            mHeight = height;
        }
    }

    void TrackBallInteractor::setScrollDirection(bool up)
    {
        mIsScrolling = true;
        float inc = mSpeed * (up ? -1.f : 1.f);
        mZoomSum += inc;
        mRollSum += inc;
    }

    void TrackBallInteractor::setSpeed(float s)
    {
        cout << "speed " << s << endl;
        mSpeed = s;
    }

    void TrackBallInteractor::update()
    {
        const bool isClick = mIsLeftClick || mIsMiddleClick || mIsRightClick;

        if (! mIsDragging)
        {
            if (isClick)
            {
                mIsDragging = true;
                computePointOnSphere(mClickPoint, mStartVector);
            } else if (mIsScrolling) {
                scroll();
                mIsScrolling = false;
            }
        } else
        {
            if (isClick)
            {
                drag();
            } else
            {
                mIsDragging = false;
            }
        }
    }

    void TrackBallInteractor::updateCameraEyeUp(bool eye, bool up)
    {
        if (eye) {
            glm::vec3 eye;
            computeCameraEye(eye);
            mCamera->setEye(eye);
        }
        if (up) {
            glm::vec3 up;
            computeCameraUp(up);
            mCamera->setUp(up);
        }
        mCamera->update();
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
