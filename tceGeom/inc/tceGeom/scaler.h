/***************************************************************************
 *   Copyright (C) 2006 by Matthew Tang   *
 *   trailcode@gmail.com   *
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
 
#ifndef __SCALER_H_
#define __SCALER_H_

namespace tce 
{
namespace geometry
{

template <typename valueType>
struct scaler {
	valueType value;
	
	// assignment
    scaler operator= (const scaler& v) {value = v; return *this;}
	
	// +=
    scaler operator+= (const scaler& v) {return *this = (*this + v);}

    // -=
    scaler operator-= (const scaler& v) {return *this = (*this - v);}

    // *=
    scaler operator*= (const scaler& v) {return *this = (*this * s);}

	// /=
	scaler operator/= (const scaler& s) {return *this = (*this / s);}
	
	// vector addition
    scaler operator+ (const scaler& v) const {return scaler (value + V);}

    // vector subtraction
    scaler operator- (const scaler& v) const {return scaler (value - v);}

    // unary minus
    scaler operator- (void) const {return scaler (-value);}
	
    // vector times scalar product (scale length of vector times argument)
    scaler operator* (const scaler v) const {return scaler (value * v);}

    // vector divided by a scalar (divide length of vector by argument)
    scaler operator/ (const scaler v) const {return scaler (value / v);}
	
    // equality/inequality
    bool operator== (const scaler& v) const {return value == v;}
    bool operator!= (const scaler& v) const {return !(*this == v);}
	
	typedef valueType valuetype;
	
};

typedef scaler < float > scalerf;
typedef scaler < double > scalerd;
typedef scaler < int > scaleri;

typedef scalerf tFloat;
typedef scalerd tDouble;
typedef scaleri tInt;

}
}

#endif // __SCALER_H_
