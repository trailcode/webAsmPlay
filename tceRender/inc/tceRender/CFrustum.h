//
// File: src/CFrustum.h
// Created by: mltang <mltang@trailCode>
// Created on: Sun Mar 23 16:27:04 2003
//

#ifndef _CFRUSTUM_H_
#define _CFRUSTUM_H_

#include <tceGeom/vec3.h>

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide
{
    RIGHT   = 0,        // The RIGHT side of the frustum
    LEFT    = 1,        // The LEFT  side of the frustum
    BOTTOM  = 2,        // The BOTTOM side of the frustum
    TOP     = 3,        // The TOP side of the frustum
    BACK    = 4,        // The BACK side of the frustum
    FRONT   = 5         // The FRONT side of the frustum
};

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData
{
    A = 0,              // The X value of the plane's normal
    B = 1,              // The Y value of the plane's normal
    C = 2,              // The Z value of the plane's normal
    D = 3               // The distance the plane is from the origin
};

class CFrustum
{
	public:
		CFrustum();
		virtual ~CFrustum();
		void CalculateFrustum();
		bool AABBInFrustum(const tce::geom::Vec3f & pos, const tce::geom::Vec3f & AABB);
		// This holds the A B C and D values for each side of our frustum.
    		float m_Frustum[6][4];
	protected:

	private:
		void NormalizePlane(float frustum[6][4], int side);


};


#endif	//_CFRUSTUM_H_

