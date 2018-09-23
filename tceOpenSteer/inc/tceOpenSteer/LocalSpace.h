// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2005, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------
//
//
// LocalSpace: a local coordinate system for 3d space
//
// Provide functionality such as transforming from local space to global
// space and vice versa.  Also regenerates a valid space from a perturbed
// "forward vector" which is the basis of abnstract vehicle turning.
//
// These are comparable to a 4x4 homogeneous transformation matrix where the
// 3x3 (R) portion is constrained to be a pure rotation (no shear or scale).
// The rows of the 3x3 R matrix are the basis vectors of the space.  They are
// all constrained to be mutually perpendicular and of unit length.  The top
// ("x") row is called "side", the middle ("y") row is called "up" and the
// bottom ("z") row is called forward.  The translation vector is called
// "position".  Finally the "homogeneous column" is always [0 0 0 1].
//
//     [ R R R  0 ]      [ Sx Sy Sz  0 ]
//     [ R R R  0 ]      [ Ux Uy Uz  0 ]
//     [ R R R  0 ]  ->  [ Fx Fy Fz  0 ]
//     [          ]      [             ]
//     [ T T T  1 ]      [ Tx Ty Tz  1 ]
//
// This file defines three classes:
//   AbstractLocalSpace:  pure virtual interface
//   LocalSpaceMixin:     mixin to layer LocalSpace functionality on any base
//   LocalSpace:          a concrete object (can be instantiated)
//
// 10-04-04 bk:  put everything into the OpenSteer namespace
// 06-05-02 cwr: created 
//
//
// ----------------------------------------------------------------------------


#ifndef OPENSTEER_LOCALSPACE_H
#define OPENSTEER_LOCALSPACE_H

#include <tceGeom/vec3.h>

// ----------------------------------------------------------------------------


namespace OpenSteer {


    class AbstractLocalSpace
    {
    public:
        virtual ~AbstractLocalSpace() { /* Nothing to do. */ }
        

        // accessors (get and set) for side, up, forward and position
        virtual tce::geom::Vec3f side (void) const = 0;
        virtual tce::geom::Vec3f setSide (tce::geom::Vec3f s) = 0;
        virtual tce::geom::Vec3f up (void) const = 0;
        virtual tce::geom::Vec3f setUp (tce::geom::Vec3f u) = 0;
        virtual tce::geom::Vec3f forward (void) const = 0;
        virtual tce::geom::Vec3f setForward (tce::geom::Vec3f f) = 0;
        virtual tce::geom::Vec3f position (void) const = 0;
        virtual tce::geom::Vec3f setPosition (tce::geom::Vec3f p) = 0;

        // use right-(or left-)handed coordinate space
        virtual bool rightHanded (void) const = 0;

        // reset transform to identity
        virtual void resetLocalSpace (void) = 0;

        // transform a direction in global space to its equivalent in local space
        virtual tce::geom::Vec3f localizeDirection (const tce::geom::Vec3f& globalDirection) const = 0;

        // transform a point in global space to its equivalent in local space
        virtual tce::geom::Vec3f localizePosition (const tce::geom::Vec3f& globalPosition) const = 0;

        // transform a point in local space to its equivalent in global space
        virtual tce::geom::Vec3f globalizePosition (const tce::geom::Vec3f& localPosition) const = 0;

        // transform a direction in local space to its equivalent in global space
        virtual tce::geom::Vec3f globalizeDirection (const tce::geom::Vec3f& localDirection) const = 0;

        // set "side" basis vector to normalized cross product of forward and up
        virtual void setUnitSideFromForwardAndUp (void) = 0;

        // regenerate the orthonormal basis vectors given a new forward
        // (which is expected to have unit length)
        virtual void regenerateOrthonormalBasisUF (const tce::geom::Vec3f& newUnitForward) = 0;

        // for when the new forward is NOT of unit length
        virtual void regenerateOrthonormalBasis (const tce::geom::Vec3f& newForward) = 0;

        // for supplying both a new forward and and new up
        virtual void regenerateOrthonormalBasis (const tce::geom::Vec3f& newForward,
                                                 const tce::geom::Vec3f& newUp) = 0;

        // rotate 90 degrees in the direction implied by rightHanded()
        virtual tce::geom::Vec3f localRotateForwardToSide (const tce::geom::Vec3f& v) const = 0;
        virtual tce::geom::Vec3f globalRotateForwardToSide (const tce::geom::Vec3f& globalForward) const=0;
    };


    // ----------------------------------------------------------------------------
    // LocalSpaceMixin is a mixin layer, a class template with a paramterized base
    // class.  Allows "LocalSpace-ness" to be layered on any class.


    template <class Super>
    class LocalSpaceMixin : public Super
    {
        // transformation as three orthonormal unit basis vectors and the
        // origin of the local space.  These correspond to the "rows" of
        // a 3x4 transformation matrix with [0 0 0 1] as the final column

    private:

        tce::geom::Vec3f _side;     //    side-pointing unit basis vector
        tce::geom::Vec3f _up;       //  upward-pointing unit basis vector
        tce::geom::Vec3f _forward;  // forward-pointing unit basis vector
        tce::geom::Vec3f _position; // origin of local space

    public:

        // accessors (get and set) for side, up, forward and position
        tce::geom::Vec3f side     (void) const {return _side;};
        tce::geom::Vec3f up       (void) const {return _up;};
        tce::geom::Vec3f forward  (void) const {return _forward;};
        tce::geom::Vec3f position (void) const {return _position;};
        tce::geom::Vec3f setSide     (tce::geom::Vec3f s) {return _side = s;};
        tce::geom::Vec3f setUp       (tce::geom::Vec3f u) {return _up = u;};
        tce::geom::Vec3f setForward  (tce::geom::Vec3f f) {return _forward = f;};
        tce::geom::Vec3f setPosition (tce::geom::Vec3f p) {return _position = p;};
        tce::geom::Vec3f setSide     (float x, float y, float z){return _side.set    (x,y,z);};
        tce::geom::Vec3f setUp       (float x, float y, float z){return _up.set      (x,y,z);};
        tce::geom::Vec3f setForward  (float x, float y, float z){return _forward.set (x,y,z);};
        tce::geom::Vec3f setPosition (float x, float y, float z){return _position.set(x,y,z);};


        // ------------------------------------------------------------------------
        // Global compile-time switch to control handedness/chirality: should
        // LocalSpace use a left- or right-handed coordinate system?  This can be
        // overloaded in derived types (e.g. vehicles) to change handedness.

        bool rightHanded (void) const {return true;}


        // ------------------------------------------------------------------------
        // constructors


        LocalSpaceMixin (void)
        {
            resetLocalSpace ();
        };

        LocalSpaceMixin (const tce::geom::Vec3f& Side,
                         const tce::geom::Vec3f& Up,
                         const tce::geom::Vec3f& Forward,
                         const tce::geom::Vec3f& Position)
            : _side( Side ), _up( Up ), _forward( Forward ), _position( Position ) {}


        LocalSpaceMixin (const tce::geom::Vec3f& Up,
                         const tce::geom::Vec3f& Forward,
                         const tce::geom::Vec3f& Position)
            : _side(), _up( Up ), _forward( Forward ), _position( Position )
        {
            setUnitSideFromForwardAndUp ();
        }

        
        virtual ~LocalSpaceMixin() { /* Nothing to do. */ }
        

        // ------------------------------------------------------------------------
        // reset transform: set local space to its identity state, equivalent to a
        // 4x4 homogeneous transform like this:
        //
        //     [ X 0 0 0 ]
        //     [ 0 1 0 0 ]
        //     [ 0 0 1 0 ]
        //     [ 0 0 0 1 ]
        //
        // where X is 1 for a left-handed system and -1 for a right-handed system.

        void resetLocalSpace (void)
        {
            _forward.set (0, 0, 1);
            _side = localRotateForwardToSide (_forward);
            _up.set (0, 1, 0);
            _position.set (0, 0, 0);
        };


        // ------------------------------------------------------------------------
        // transform a direction in global space to its equivalent in local space


        tce::geom::Vec3f localizeDirection (const tce::geom::Vec3f& globalDirection) const
        {
            // dot offset with local basis vectors to obtain local coordiantes
            return tce::geom::Vec3f (globalDirection.dot (_side),
                         globalDirection.dot (_up),
                         globalDirection.dot (_forward));
        };


        // ------------------------------------------------------------------------
        // transform a point in global space to its equivalent in local space


        tce::geom::Vec3f localizePosition (const tce::geom::Vec3f& globalPosition) const
        {
            // global offset from local origin
            tce::geom::Vec3f globalOffset = globalPosition - _position;

            // dot offset with local basis vectors to obtain local coordiantes
            return localizeDirection (globalOffset);
        };


        // ------------------------------------------------------------------------
        // transform a point in local space to its equivalent in global space


        tce::geom::Vec3f globalizePosition (const tce::geom::Vec3f& localPosition) const
        {
            return _position + globalizeDirection (localPosition);
        };


        // ------------------------------------------------------------------------
        // transform a direction in local space to its equivalent in global space


        tce::geom::Vec3f globalizeDirection (const tce::geom::Vec3f& localDirection) const
        {
            return ((_side    * localDirection.x) +
                    (_up      * localDirection.y) +
                    (_forward * localDirection.z));
        };


        // ------------------------------------------------------------------------
        // set "side" basis vector to normalized cross product of forward and up


        void setUnitSideFromForwardAndUp (void)
        {
            // derive new unit side basis vector from forward and up
            if (rightHanded())
                _side.cross (_forward, _up);
            else
                _side.cross (_up, _forward);
            _side = _side.normalize ();
        }


        // ------------------------------------------------------------------------
        // regenerate the orthonormal basis vectors given a new forward
        // (which is expected to have unit length)


        void regenerateOrthonormalBasisUF (const tce::geom::Vec3f& newUnitForward)
        {
            _forward = newUnitForward;

            // derive new side basis vector from NEW forward and OLD up
            setUnitSideFromForwardAndUp ();

            // derive new Up basis vector from new Side and new Forward
            // (should have unit length since Side and Forward are
            // perpendicular and unit length)
            if (rightHanded())
                _up.cross (_side, _forward);
            else
                _up.cross (_forward, _side);
        }


        // for when the new forward is NOT know to have unit length

        void regenerateOrthonormalBasis (const tce::geom::Vec3f& newForward)
        {
            regenerateOrthonormalBasisUF (newForward.normalize());
        }


        // for supplying both a new forward and and new up

        void regenerateOrthonormalBasis (const tce::geom::Vec3f& newForward,
                                         const tce::geom::Vec3f& newUp)
        {
            _up = newUp;
            regenerateOrthonormalBasis (newForward.normalize());
        }


        // ------------------------------------------------------------------------
        // rotate, in the canonical direction, a vector pointing in the
        // "forward" (+Z) direction to the "side" (+/-X) direction


        tce::geom::Vec3f localRotateForwardToSide (const tce::geom::Vec3f& v) const
        {
            return tce::geom::Vec3f (rightHanded () ? -v.z : +v.z,
                         v.y,
                         v.x);
        }

        // not currently used, just added for completeness

        tce::geom::Vec3f globalRotateForwardToSide (const tce::geom::Vec3f& globalForward) const
        {
            const tce::geom::Vec3f localForward = localizeDirection (globalForward);
            const tce::geom::Vec3f localSide = localRotateForwardToSide (localForward);
            return globalizeDirection (localSide);
        }
    };


    // ----------------------------------------------------------------------------
    // Concrete LocalSpace class, and a global constant for the identity transform


    typedef LocalSpaceMixin<AbstractLocalSpace> LocalSpace;

    const LocalSpace gGlobalSpace;

} // namespace OpenSteer

// ----------------------------------------------------------------------------
#endif // OPENSTEER_LOCALSPACE_H


