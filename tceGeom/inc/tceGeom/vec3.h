/***************************************************************************
 *   Copyright (C) 2006 by Matthew Tang                                    *
 *   trailcode@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __VEC3_H_
#define __VEC3_H_

#include <stdio.h>
#include <assert.h>
#include <ostream>

#include <tceOpenSteer/Utilities.h>

#ifndef DIST_EPSILON
#define DIST_EPSILON (float(1.0f / 32.0f))
#endif

#ifndef CONVERT_ENDIAN_UINT32
#define CONVERT_ENDIAN_UINT32(x) x = ((((unsigned int)(x)&0xff000000) >> 24) | \
									  (((unsigned int)(x)&0x00ff0000) >> 8) |  \
									  (((unsigned int)(x)&0x0000ff00) << 8) |  \
									  (((unsigned int)(x)&0x000000ff) << 24))
#endif

namespace tce
{
namespace geom
{

template <typename valueType>
struct Vec3
{
	// ----------------------------------------- generic 3d vector operations

	// three-dimensional Cartesian coordinates
	valueType x, y, z;

	// constructors
	Vec3() : x(0), y(0), z(0) { ; }

	Vec3(valueType _x, valueType _y, valueType _z) : x(_x), y(_y), z(_z) { ; }

	Vec3(valueType p[]) : x(p[0]), y(p[1]), z(p[2]) { ; }

	//Vec3(const Vec3 & p) : x(p[0]), y(p[1]), z(p[2]) { ;}

	Vec3(const Vec3<double> &p) : x(p[0]), y(p[1]), z(p[2]) { ; }
	Vec3(const Vec3<float> &p) : x(p[0]), y(p[1]), z(p[2]) { ; }
	Vec3(const Vec3<int> &p) : x(p[0]), y(p[1]), z(p[2]) { ; }

	//Vec3(const Vec3 * p) : x(p->x), y(p->y), z(p->z) { ;}

	valueType getX() const { return x; }
	valueType getY() const { return y; }
	valueType getZ() const { return z; }

	valueType setX(const valueType x) { return this->x = x; }
	valueType setY(const valueType y) { return this->y = y; }
	valueType setZ(const valueType z) { return this->z = z; }

	// dot product
	valueType dot(const Vec3 &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

	// length
	valueType length(void) const { return sqrt(lengthSquared()); }

	valueType distance(const Vec3 &v) const { return (*this - v).length(); }

	// length squared
	valueType lengthSquared(void) const { return this->dot(*this); }

	// normalize: returns normalized version (parallel to this, length = 1)
	Vec3 normalize(void) const
	{
		// skip divide if length is zero
		const valueType len = length();
		return (len > 0) ? (*this) / len : (*this);
	}

	// cross product (modify "*this" to be A x B)
	// [XXX  side effecting -- deprecate this function?  XXX]
	Vec3 cross(const Vec3 &a, const Vec3 &b)
	{
		return *this = Vec3((a.y * b.z) - (a.z * b.y),
							(a.z * b.x) - (a.x * b.z),
					 		(a.x * b.y) - (a.y * b.x));
	}

	inline static void Normalize(Vec3 &vec)
	{
		const valueType len = vec.length();
		if (len > 0)
		{
			vec = vec / len;
		}
	}

	inline static valueType Dot_Product(const Vec3 &a, const Vec3 &b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	inline static void CrossProduct(const Vec3 &v1, const Vec3 &v2, Vec3 &cross)
	{
		cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
		cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
		cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
	}

	inline static Vec3 CrossProduct(const Vec3 &v1, const Vec3 &v2)
	{
		Vec3 cross;
		cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
		cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
		cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
		return cross;
	}

	inline static valueType Length(const Vec3 &vec)
	{
		return vec.length();
	}

	inline static void VectorScale(const Vec3 &v, valueType scale, Vec3 &out)
	{
		out = v * scale;
	}

	inline static float Distance(const Vec3 &A, const Vec3 &B)
	{
		return (A - B).length();
	}

	inline static Vec3 interpolate(float alpha, Vec3 &x0, Vec3 &x1)
	{
		return x0 + ((x1 - x0) * alpha);
	}

	// ----------------------------------------------------------------------------
	// Returns a position randomly distributed inside a sphere of unit radius
	// centered at the origin.  Orientation will be random and length will range
	// between 0 and 1
	inline static Vec3 RandomVectorInUnitRadiusSphere(void)
	{
		Vec3 v;

		do
		{
			v.set((OpenSteer::frandom01() * 2) - 1,
				  (OpenSteer::frandom01() * 2) - 1,
				  (OpenSteer::frandom01() * 2) - 1);
		} while (v.length() >= 1);

		return v;
	}

	// ----------------------------------------------------------------------------
	// Returns a position randomly distributed on a disk of unit radius
	// on the XZ (Y=0) plane, centered at the origin.  Orientation will be
	// random and length will range between 0 and 1

	inline static Vec3 randomVectorOnUnitRadiusXZDisk(void)
	{
		Vec3 v((OpenSteer::frandom01() * 2) - 1, 0, (OpenSteer::frandom01() * 2) - 1);

		Vec3::Normalize(v);

		return v;
	}

	// ----------------------------------------------------------------------------
	// Returns a position randomly distributed on a circle of unit radius
	// on the XZ (Y=0) plane, centered at the origin.  Orientation will be
	// random and length will be 1

	inline static Vec3 randomUnitVectorOnXZPlane(void)
	{
		//return randomVectorInUnitRadiusSphere().setYtoZero().normalize();

		Vec3 v;

		do
		{
			v.set((OpenSteer::frandom01() * 2) - 1,
				  0,
				  (OpenSteer::frandom01() * 2) - 1);
		} while (v.length() >= 1);

		return v;
	}

	// ----------------------------------------------------------------------------
	// Does a "ceiling" or "floor" operation on the angle by which a given vector
	// deviates from a given reference basis vector.  Consider a cone with "basis"
	// as its axis and slope of "cosineOfConeAngle".  The first argument controls
	// whether the "source" vector is forced to remain inside or outside of this
	// cone.  Called by vecLimitMaxDeviationAngle and vecLimitMinDeviationAngle.

	inline static Vec3 vecLimitDeviationAngleUtility(const bool insideOrOutside,
													 const Vec3 &source,
													 const float cosineOfConeAngle,
													 const Vec3 &basis)
	{
		// immediately return zero length input vectors
		float sourceLength = source.length();
		if (sourceLength == 0)
			return source;

		// measure the angular diviation of "source" from "basis"
		const Vec3 direction = source / sourceLength;
		float cosineOfSourceAngle = direction.dot(basis);

		// Simply return "source" if it already meets the angle criteria.
		// (note: we hope this top "if" gets compiled out since the flag
		// is a constant when the function is inlined into its caller)
		if (insideOrOutside)
		{
			// source vector is already inside the cone, just return it
			if (cosineOfSourceAngle >= cosineOfConeAngle)
				return source;
		}
		else
		{
			// source vector is already outside the cone, just return it
			if (cosineOfSourceAngle <= cosineOfConeAngle)
				return source;
		}

		// find the portion of "source" that is perpendicular to "basis"
		const Vec3 perp = source.perpendicularComponent(basis);

		// normalize that perpendicular
		const Vec3 unitPerp = perp.normalize();

		// construct a new vector whose length equals the source vector,
		// and lies on the intersection of a plane (formed the source and
		// basis vectors) and a cone (whose axis is "basis" and whose
		// angle corresponds to cosineOfConeAngle)
		float perpDist = OpenSteer::sqrtXXX(1 - (cosineOfConeAngle * cosineOfConeAngle));
		const Vec3 c0 = basis * cosineOfConeAngle;
		const Vec3 c1 = unitPerp * perpDist;
		return (c0 + c1) * sourceLength;
	}

	// ----------------------------------------------------------------------------
	// given a vector, return a vector perpendicular to it.  arbitrarily selects
	// one of the infinitely many perpendicular vectors.  a zero vector maps to
	// itself, otherwise length is irrelevant (empirically, output length seems to
	// remain within 20% of input length).

	inline static Vec3 findPerpendicularIn3d(const Vec3 &direction)
	{
		// to be filled in:
		Vec3 quasiPerp; // a direction which is "almost perpendicular"
		Vec3 result;	// the computed perpendicular to be returned

		// three mutually perpendicular basis vectors
		const Vec3 i(1, 0, 0);
		const Vec3 j(0, 1, 0);
		const Vec3 k(0, 0, 1);

		// measure the projection of "direction" onto each of the axes
		const float id = i.dot(direction);
		const float jd = j.dot(direction);
		const float kd = k.dot(direction);

		// set quasiPerp to the basis which is least parallel to "direction"
		if ((id <= jd) && (id <= kd))
		{
			quasiPerp = i; // projection onto i was the smallest
		}
		else
		{
			if ((jd <= id) && (jd <= kd))
				quasiPerp = j; // projection onto j was the smallest
			else
				quasiPerp = k; // projection onto k was the smallest
		}

		// return the cross product (direction x quasiPerp)
		// which is guaranteed to be perpendicular to both of them
		result.cross(direction, quasiPerp);
		return result;
	}

	// ----------------------------------------------------------------------------
	// Returns a position randomly distributed on the surface of a sphere
	// of unit radius centered at the origin.  Orientation will be random
	// and length will be 1

	inline static Vec3 RandomUnitVector(void)
	{
		return RandomVectorInUnitRadiusSphere().normalize();
	}

	// ----------------------------------------------------------------------------
	// Returns a position randomly distributed on a circle of unit radius
	// on the XZ (Y=0) plane, centered at the origin.  Orientation will be
	// random and length will be 1

	inline static Vec3 RandomUnitVectorOnXZPlane(void)
	{
		return RandomVectorInUnitRadiusSphere().setYtoZero().normalize();
	}

	// ----------------------------------------------------------------------------
	// Enforce an upper bound on the angle by which a given arbitrary vector
	// diviates from a given reference direction (specified by a unit basis
	// vector).  The effect is to clip the "source" vector to be inside a cone
	// defined by the basis and an angle.

	inline static Vec3 limitMaxDeviationAngle(const Vec3 &source,
											  const float cosineOfConeAngle,
											  const Vec3 &basis)
	{
		return vecLimitDeviationAngleUtility(true, // force source INSIDE cone
											 source,
											 cosineOfConeAngle,
											 basis);
	}

	// ----------------------------------------------------------------------------
	// Enforce a lower bound on the angle by which a given arbitrary vector
	// diviates from a given reference direction (specified by a unit basis
	// vector).  The effect is to clip the "source" vector to be outside a cone
	// defined by the basis and an angle.

	inline static Vec3 limitMinDeviationAngle(const Vec3 &source,
											  const float cosineOfConeAngle,
											  const Vec3 &basis)
	{
		return vecLimitDeviationAngleUtility(false, // force source OUTSIDE cone
											 source,
											 cosineOfConeAngle,
											 basis);
	}

	// ----------------------------------------------------------------------------
	// Returns the distance between a point and a line.  The line is defined in
	// terms of a point on the line ("lineOrigin") and a UNIT vector parallel to
	// the line ("lineUnitTangent")

	inline static float distanceFromLine(const Vec3 &point,
										 const Vec3 &lineOrigin,
										 const Vec3 &lineUnitTangent)
	{
		const Vec3 offset = point - lineOrigin;
		const Vec3 perp = offset.perpendicularComponent(lineUnitTangent);
		return perp.length();
	}

	inline static void
	RotateVecAxisY(Vec3 &V, const float &ang)
	{
		const float tmpX = V.x;
		V.x = (cos(ang) * V.x) - (sin(ang) * V.z);
		V.z = (cos(ang) * V.z) + (sin(ang) * tmpX);
	}

	// assignment
	Vec3 &operator=(const Vec3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// set XYZ coordinates to given three valueTypes
	Vec3 set(const valueType _x, const valueType _y, const valueType _z)
	{
		x = _x;
		y = _y;
		z = _z;
		return *this;
	}

	// +=
	Vec3 operator+=(const Vec3 &v) { return *this = (*this + v); }

	// -=
	Vec3 operator-=(const Vec3 &v) { return *this = (*this - v); }

	// *=
	Vec3 operator*=(const valueType &s) { return *this = (*this * s); }

	// /=
	Vec3 operator/=(const valueType &s) { return *this = (*this / s); }

	// vector addition
	Vec3 operator+(const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

	// vector subtraction
	Vec3 operator-(const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

	// unary minus
	Vec3 operator-(void) const { return Vec3(-x, -y, -z); }

	// vector times scalar product (scale length of vector times argument)
	Vec3 operator*(const valueType s) const { return Vec3(x * s, y * s, z * s); }

	// vector divided by a scalar (divide length of vector by argument)
	Vec3 operator/(const valueType s) const { return Vec3(x / s, y / s, z / s); }

	valueType &operator[](const unsigned int i) const { return *&((valueType *)this)[i]; }

	// equality/inequality
	bool operator==(const Vec3 &v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const Vec3 &v) const { return !(*this == v); }

	bool operator<(const Vec3 &v) const { return x < v.x && y < v.y && z < v.z; }
	bool operator>(const Vec3 &v) const { return x > v.x && y > v.y && z > v.z; }
	bool operator<=(const Vec3 &v) const { return x <= v.x && y <= v.y && z <= v.z; }
	bool operator>=(const Vec3 &v) const { return x >= v.x && y >= v.y && z >= v.z; }

	// XXX experimental (4-1-03 cwr): is this the right approach?  defining
	// XXX "Vec3 distance (vec3, Vec3)" collided with STL's distance template.
	//inline static valueType distance (const Vec3& a, const Vec3& b){return(a-b).length();}

	// --------------------------- utility member functions used in OpenSteer

	// return component of vector parallel to a unit basis vector
	// (IMPORTANT NOTE: assumes "basis" has unit magnitude (length==1))

	inline Vec3 parallelComponent(const Vec3 &unitBasis) const
	{
		const valueType projection = this->dot(unitBasis);
		return unitBasis * projection;
	}

	// return component of vector perpendicular to a unit basis vector
	// (IMPORTANT NOTE: assumes "basis" has unit magnitude (length==1))

	inline Vec3 perpendicularComponent(const Vec3 &unitBasis) const
	{
		return (*this) - parallelComponent(unitBasis);
	}

	// clamps the length of a given vector to maxLength.  If the vector is
	// shorter its value is returned unaltered, if the vector is longer
	// the value returned has length of maxLength and is paralle to the
	// original input.

	Vec3 truncateLength(const valueType maxLength) const
	{
		const valueType maxLengthSquared = maxLength * maxLength;
		const valueType vecLengthSquared = this->lengthSquared();
		if (vecLengthSquared <= maxLengthSquared)
			return *this;
		else
			return (*this) * (maxLength / sqrt(vecLengthSquared));
	}

	// forces a 3d position onto the XZ (aka y=0) plane

	Vec3 setYtoZero(void) const { return Vec3(this->x, 0, this->z); }

	// rotate this vector about the global Y (up) axis by the given angle

	Vec3 rotateAboutGlobalY(valueType angle) const
	{
		const valueType s = sinXXX(angle);
		const valueType c = cosXXX(angle);
		return Vec3((this->x * c) + (this->z * s),
					(this->y),
					(this->z * c) - (this->x * s));
	}

	bool fuzzyCompare(const Vec3 &cmpVec, const float &tolerance)
	{

		if (fabs(x - cmpVec.x) > tolerance)
			return false;
		if (fabs(y - cmpVec.y) > tolerance)
			return false;
		if (fabs(z - cmpVec.z) > tolerance)
			return false;

		return true;
	}

	void print(void) const
	{
		printf("(%f,%f,%f)\r\n", x, y, z);
	}

	typedef valueType valuetype;

	inline Vec3 glVertex() const
	{
		glVertex3f(x, y, z);

		return *this;
	}

	inline Vec3 glScale() const
	{
		glScalef(x, y, z);

		return *this;
	}

	inline Vec3 glTranslate() const
	{
		glTranslatef(x, y, z);

		return *this;
	}

	inline Vec3 glColor() const
	{
		glColor3f(x, y, z);

		return *this;
	}

	void convertEndian()
	{
		// @@@ Make this work for short, etc
		assert(sizeof(valuetype) == sizeof(unsigned int));

		unsigned int tmp = *(reinterpret_cast<unsigned int *>(&x));
		CONVERT_ENDIAN_UINT32(tmp);

		x = *(reinterpret_cast<valuetype *>(&tmp));

		tmp = *(reinterpret_cast<unsigned int *>(&y));
		CONVERT_ENDIAN_UINT32(tmp);

		y = *(reinterpret_cast<valuetype *>(&tmp));

		tmp = *(reinterpret_cast<unsigned int *>(&z));
		CONVERT_ENDIAN_UINT32(tmp);

		z = *(reinterpret_cast<valuetype *>(&tmp));
	}

	Vec3 swapYandZ() const
	{
		Vec3 ret(this);
		valueType temp = ret.y;
		ret.y = ret.z;
		return ret;
	}

	Vec3 setSwapYandZ()
	{
		valueType temp = y;
		y = z;
		return *this;
	}

	Vec3 invertZ() const
	{
		Vec3 ret(this);
		ret.z = -ret.z;
		return ret;
	}

	Vec3 setInvertZ()
	{
		z = -z;
		return *this;
	}

	// names for frequently used vector constants
	static const Vec3 zero;
	static const Vec3 side;
	static const Vec3 up;
	static const Vec3 forward;
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

// ----------------------------------------------------------------------------
// scalar times vector product ("float * Vec3")

Vec3f inline operator*(float s, const Vec3f &v) { return v * s; }
Vec3d inline operator*(double s, const Vec3d &v) { return v * s; }
Vec3i inline operator*(int s, const Vec3i &v) { return v * s; }
} // namespace geom
} // namespace tce

template <typename T>
std::ostream& operator<<(std::ostream& os, const tce::geom::Vec3<T> & v)  
{  
    os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
	
    return os;  
}  

// @@@ There must be a better way
#define VEC3_TEMPLATE_INIT \
	// ---------------------------------------------------------------------------- \
	// names for frequently used vector constants                                   \
	                                                                                \
	template<> const tce::geom::Vec3f tce::geom::Vec3f::zero    (0, 0, 0);  \
	template<> const tce::geom::Vec3f tce::geom::Vec3f::up      (0, 1, 0);  \
	template<> const tce::geom::Vec3f tce::geom::Vec3f::forward (0, 0, 1);  \
																					\
	// XXX  This should be unified with LocalSpace::rightHanded, but I don't want   \
	// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there	\
	// XXX  should be a tiny chirality.h header to define a const?  That could		\
	// XXX  then be included by both Vec3.h and LocalSpace.h						\
																					\
	template<> const tce::geom::Vec3f tce::geom::Vec3f::side    (-1, 0, 0); \
																					\
	// ---------------------------------------------------------------------------- \
	// names for frequently used vector constants									\
																					\
	template<> const tce::geom::Vec3d tce::geom::Vec3d::zero    (0, 0, 0);  \
	template<> const tce::geom::Vec3d tce::geom::Vec3d::up      (0, 1, 0);  \
	template<> const tce::geom::Vec3d tce::geom::Vec3d::forward (0, 0, 1);  \
																					\
	// XXX  This should be unified with LocalSpace::rightHanded, but I don't want	\
	// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there	\
	// XXX  should be a tiny chirality.h header to define a const?  That could		\
	// XXX  then be included by both Vec3.h and LocalSpace.h						\
																					\
	template<> const tce::geom::Vec3d tce::geom::Vec3d::side    (-1, 0, 0); \
																					\
	// ----------------------------------------------------------------------------	\
	// names for frequently used vector constants									\
																					\
	template<> const tce::geom::Vec3i tce::geom::Vec3i::zero    (0, 0, 0);	\
	template<> const tce::geom::Vec3i tce::geom::Vec3i::up      (0, 1, 0);	\
	template<> const tce::geom::Vec3i tce::geom::Vec3i::forward (0, 0, 1);	\
																					\
	// XXX  This should be unified with LocalSpace::rightHanded, but I don't want	\
	// XXX  Vec3 to be based on LocalSpace which is based on Vec3.  Perhaps there	\
	// XXX  should be a tiny chirality.h header to define a const?  That could		\
	// XXX  then be included by both Vec3.h and LocalSpace.h						\
																					\
	template<> const tce::geom::Vec3i tce::geom::Vec3i::side    (-1, 0, 0);

#endif // __VEC3_H_
