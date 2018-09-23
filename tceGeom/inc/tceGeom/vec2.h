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

#ifndef _VEC2_H__
#define _VEC2_H__

#include <math.h>

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <ostream>

#ifndef DIST_EPSILON
#define DIST_EPSILON (float (1.0f/32.0f))
#endif

#ifndef CONVERT_ENDIAN_UINT32
#define CONVERT_ENDIAN_UINT32(x)   x = 	 ((((unsigned int)(x) & 0xff000000) >> 24) | \
			                             (((unsigned int)(x) & 0x00ff0000) >>  8) | \
			                             (((unsigned int)(x) & 0x0000ff00) <<  8) | \
			                             (((unsigned int)(x) & 0x000000ff) << 24))
#endif

namespace tce 
{
	namespace geom
	{
		template <typename valueType>
		struct Vec2
		{
		
			// ----------------------------------------- generic 2d vector operations
		
			valueType x,y;
		
			// constructors
			Vec2 (void) {x = y = 0;}
			Vec2 (valueType X, valueType Y) {x = X; y = Y ;}
			Vec2 (valueType P[]) { x = P[0], y = P[1] ;}
			Vec2 (const Vec2<int> & v) { x = valueType(v[0]); y = valueType(v[1]) ;}
			Vec2 (const Vec2<long> & v) { x = valueType(v[0]); y = valueType(v[1]) ;}
			Vec2 (const Vec2<float> & v) { x = valueType(v[0]); y = valueType(v[1]) ;}
			Vec2 (const Vec2<double> & v) { x = valueType(v[0]); y = valueType(v[1]) ;}
			
			// dot product
			valueType dot (const Vec2& v) const {return (x * v.x) + (y * v.y) ;}
		
			// length
			valueType length (void) const {return valueType(sqrt(double(lengthSquared())));}
		
			// length squared
			valueType lengthSquared (void) const {return this->dot (*this);}
		
			// normalize: returns normalized version (parallel to this, length = 1)
			Vec2 normalize (void) const
			{
				// skip divide if length is zero
				const valueType len = length ();
				return (len>0) ? (*this)/len : (*this);
			}
		
			inline static void Normalize(Vec2 & vec)
			{
				const valueType len = vec.length ();
				if(len > 0) { vec = vec / len ;}
			}
		
			inline static valueType Dot_Product(const Vec2 & a, const Vec2 & b)
			{
				return (a.x * b.x) + (a.y * b.y);
			}
		
			inline static valueType Length(const Vec2 & vec)
			{
				return vec.length();
			}
		
			inline static void VectorScale (const Vec2 & v, valueType scale, Vec2 & out)
			{
				out = v * scale;
			}
		
			inline static float Distance(const Vec2 & A, const Vec2 & B)
			{
				return(A-B).length();
			}
		
			inline static Vec2 interpolate (float alpha, Vec2 & x0, Vec2 & x1)
			{
				return x0 + ((x1 - x0) * valueType(alpha));
			}
		
			
			// ----------------------------------------------------------------------------
			// Does a "ceiling" or "floor" operation on the angle by which a given vector
			// deviates from a given reference basis vector.  Consider a cone with "basis"
			// as its axis and slope of "cosineOfConeAngle".  The first argument controls
			// whether the "source" vector is forced to remain inside or outside of this
			// cone.  Called by vecLimitMaxDeviationAngle and vecLimitMinDeviationAngle.
			inline static Vec2 vecLimitDeviationAngleUtility (const bool insideOrOutside,
			        const Vec2& source,
			        const valueType cosineOfConeAngle,
			        const Vec2& basis)
			{
				// immediately return zero length input vectors
				valueType sourceLength = source.length();
				if (sourceLength == 0) return source;
		
				// measure the angular diviation of "source" from "basis"
				const Vec2 direction = source / sourceLength;
				valueType cosineOfSourceAngle = direction.dot (basis);
		
				// Simply return "source" if it already meets the angle criteria.
				// (note: we hope this top "if" gets compiled out since the flag
				// is a constant when the function is inlined into its caller)
				if (insideOrOutside)
				{
					// source vector is already inside the cone, just return it
					if (cosineOfSourceAngle >= cosineOfConeAngle) return source;
				}
				else
				{
					// source vector is already outside the cone, just return it
					if (cosineOfSourceAngle <= cosineOfConeAngle) return source;
				}
		
				// find the portion of "source" that is perpendicular to "basis"
				const Vec2 perp = source.perpendicularComponent (basis);
		
				// normalize that perpendicular
				const Vec2 unitPerp = perp.normalize ();
		
				// construct a new vector whose length equals the source vector,
				// and lies on the intersection of a plane (formed the source and
				// basis vectors) and a cone (whose axis is "basis" and whose
				// angle corresponds to cosineOfConeAngle)
				valueType perpDist = valueType(sqrt(1 - (cosineOfConeAngle * cosineOfConeAngle)));
				const Vec2 c0 = basis * cosineOfConeAngle;
				const Vec2 c1 = unitPerp * perpDist;
				return (c0 + c1) * sourceLength;
			}
		
			
			// ----------------------------------------------------------------------------
			// Returns the distance between a point and a line.  The line is defined in
			// terms of a point on the line ("lineOrigin") and a UNIT vector parallel to
			// the line ("lineUnitTangent")
			inline static float distanceFromLine (const Vec2& point,
			                                      const Vec2& lineOrigin,
			                                      const Vec2& lineUnitTangent)
			{
				const Vec2 offset = point - lineOrigin;
				const Vec2 perp = offset.perpendicularComponent (lineUnitTangent);
				return perp.length();
			}
		
			// assignment
			Vec2 operator= (const Vec2& v) {x=v.x; y=v.y; return *this;}
		
			// set XY coordinates to given three valueTypes
			Vec2 set (const valueType _x, const valueType _y)
			{x = _x; y = _y; return *this;}
		
			// +=
			Vec2 operator+= (const Vec2& v) {return *this = (*this + v);}
		
			// -=
			Vec2 operator-= (const Vec2& v) {return *this = (*this - v);}
		
			// *=
			Vec2 operator*= (const valueType& s) {return *this = (*this * s);}
		
			// /=
			Vec2 operator/= (const valueType& s) {return *this = (*this / s);}
		
			// vector addition
			Vec2 operator+ (const Vec2& v) const {return Vec2 (x+v.x, y+v.y);}
		
			// vector subtraction
			Vec2 operator- (const Vec2& v) const {return Vec2 (x-v.x, y-v.y);}
		
			// unary minus
			Vec2 operator- (void) const {return Vec2 (-x, -y);}
		
			// vector times scalar product (scale length of vector times argument)
			Vec2 operator* (const valueType s) const {return Vec2 (x * s, y * s);}
		
			// vector divided by a scalar (divide length of vector by argument)
			Vec2 operator/ (const valueType s) const {return Vec2 (x / s, y / s);}
		
			valueType & operator[] (const unsigned int i) const {return *&((valueType *)this)[i] ;}
		
			// equality/inequality
			bool operator== (const Vec2& v) const {return x==v.x && y==v.y ;}
			bool operator!= (const Vec2& v) const {return !(*this == v);}
		
			// XXX experimental (4-1-03 cwr): is this the right approach?  defining
			// XXX "Vec2 distance (vec3, Vec2)" collided with STL's distance template.
			inline static valueType distance (const Vec2& a, const Vec2& b){return(a-b).length();}
		
			// --------------------------- utility member functions used in OpenSteer
		
			// return component of vector parallel to a unit basis vector
			// (IMPORTANT NOTE: assumes "basis" has unit magnitude (length==1))
		
			inline Vec2 parallelComponent (const Vec2& unitBasis) const
			{
				const valueType projection = this->dot (unitBasis);
				return unitBasis * projection;
			}
		
			// return component of vector perpendicular to a unit basis vector
			// (IMPORTANT NOTE: assumes "basis" has unit magnitude (length==1))
		
			inline Vec2 perpendicularComponent (const Vec2& unitBasis) const
			{
				return (*this) - parallelComponent (unitBasis);
			}
		
			// clamps the length of a given vector to maxLength.  If the vector is
			// shorter its value is returned unaltered, if the vector is longer
			// the value returned has length of maxLength and is paralle to the
			// original input.
		
			Vec2 truncateLength (const valueType maxLength) const
			{
				const valueType maxLengthSquared = maxLength * maxLength;
				const valueType vecLengthSquared = this->lengthSquared ();
				if (vecLengthSquared <= maxLengthSquared)
					return *this;
				else
					return (*this) * (maxLength / valueType(sqrt (double(vecLengthSquared))));
			}
		
			
			Vec2 rotate (valueType angle) const
			{
				const valueType s = valueType(sin (angle));
				const valueType c = valueType(cos (angle));
				return Vec2 ((this->x * c) + (this->y * s),
				             (this->y * c) - (this->x * s));
			}
		
			bool fuzzyCompare(const Vec2 & cmpVec, const float & tolerance)
			{
		
		
				if(fabs(x - cmpVec.x) > tolerance) return false;
				if(fabs(y - cmpVec.y) > tolerance) return false;
				
				return true;
			}
		
			void print(void) const
			{
				printf("(%f,%f)\r\n",float(x),float(y));
			}
		
			inline Vec2 glVertex() const
			{
				glVertex2f(x,y);
				
				return *this;
			}
			
			inline Vec2 glScale() const
			{
				glScalef(x,y,1);
				
				return *this;
			}
			
			inline Vec2 glTranslate() const
			{
				glTranslatef(x,y,0);
				
				return *this;
			}
			
			inline Vec2 glTexCoord() const
			{
				glTexCoord2f(x,y);
				
				return *this;
			}
			
			typedef valueType valuetype;
		
			void convertEndian()
			{
				// @@@ Make this work for short, etc
				assert(sizeof(valuetype) == sizeof(unsigned int));
		
				unsigned int tmp = *(reinterpret_cast<unsigned int*>(&x));
				CONVERT_ENDIAN_UINT32(tmp);
		
				x = *(reinterpret_cast<valuetype*>(&tmp));
		
				tmp = *(reinterpret_cast<unsigned int*>(&y));
				CONVERT_ENDIAN_UINT32(tmp);
		
				y = *(reinterpret_cast<valuetype*>(&tmp));
		
			}
		};
		
		typedef Vec2 < float > Vec2f;
		typedef Vec2 < double > Vec2d;
		typedef Vec2 < int > Vec2i;
		typedef Vec2 < long > Vec2l;
		
		// ----------------------------------------------------------------------------
		// scalar times vector product ("float * Vec2")
		
		Vec2f inline operator* (float s, const Vec2f& v) {return v*s;}
		Vec2d inline operator* (double s, const Vec2d& v) {return v*s;}
		Vec2i inline operator* (int s, const Vec2i& v) {return v*s;}
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const tce::geom::Vec2<T> & v)  
{  
    os << '(' << v.x << ',' << v.y << ')';

    return os;  
}

void pyExport_Vec2_int();
void pyExport_Vec2_float();
void pyExport_Vec2_double();

#endif // _VEC2_H__
