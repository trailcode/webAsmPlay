/**
╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

\author Matthew Tang
\email trailcode@gmail.com
\copyright 2019
*/

#pragma once

#include <glm/geometric.hpp>

/**
* A plane or a 3D half space. Template parameter valueType is the number type to 
* use. 
*/
struct Plane
{
    enum
    {
        SIDE_ON = 0,
        SIDE_FRONT,
        SIDE_BACK,
        SIDE_BOTH,
    };

    /**
    * Creates a plane defined by a normal and a distance from the origin (0,0,0)
    */
	Plane(const glm::dvec3 & normal, const double dist) : normal(normal), dist(dist) { ;};

    /**
    * Creates a plane defined by any point on the plane and its normal
    */
	Plane(const glm::dvec3 & pointOnPlane, const glm::dvec3 & normal) : normal(normal), dist(glm::dot(pointOnPlane, normal)) { ;}

    /**
    * Creates a plane defined by three points. The order of point rotation, eg 
    * clockwise or counter clockwise defines what the front and back side of the 
    * plane will be. 
    */
	Plane(const glm::dvec3 & P1, const glm::dvec3 & P2, const glm::dvec3 & P3)
    {
        // Using the three point plane definition, construct the plane's normal
        normal = glm::normalize(glm::cross(P2 - P1, P3 - P1));

        // and the plane's distance from the origin.
        dist = glm::dot(normal, P1);
    }

	Plane(const double a, const double b, const double c, const double d)
	{
		// set the normal vector
		normal = glm::dvec3(a,b,c);

		//compute the lenght of the vector
		const double l = glm::length(normal);
		
		// normalize the vector
		normal = glm::dvec3(a/l,b/l,c/l);

		// and divide d by th length as well
		dist = d/l;
	}

	Plane(const Plane & plane) : normal(plane.normal), dist(plane.dist) { ;}
        
    /**
    * Creates an uninitalized plane
    */
	Plane() : normal(), dist() {;};

    ~Plane() {;}

    /**
    * Sets this plane to be exactly like plane
    */
	Plane & operator=(const Plane & plane)
    {
        dist = plane.dist;

        normal = plane.normal;

        return *this;
    }

    /**
    * Sets this plane to be the inverts of plane. That is both planes will divide 
    * a space in half exactly the same, just the normal of this plane will be 
    * facing in the opposite direction.  
    */
    void setInverted(const Plane & plane)
    {
        dist = -plane.dist;
        normal = -plane.normal;
    }

    /**
    * The planes normal
    */
    glm::dvec3 normal;

    /**
    * The planes distance from the origin (0,0,0)
    */
    double dist;

	const double ON_EPSILON = 0.00001; // TODO find a place to put this!

    /**
    * Figures out if the parameter point is lying on this plane, behind the 
    * plane, or in front of the plane. The corresponding values SIDE_ON, 
    * SIDE_BACK, or SIDE_FRONT is returned. 
    */
    int classifyPoint (const glm::dvec3 & point) const
    {
		//valueType dot = point.dot(normal) - dist;
		/*
		const double dot = glm::dot(normal, point) + dist;

		     if (dot < -ON_EPSILON) { return SIDE_FRONT ;}
		else if (dot > ON_EPSILON)  { return SIDE_BACK  ;}

		return SIDE_ON;
		*/

		//float d;
		const double d = normal.x*point.x + normal.y*point.y + normal.z*point.z + dist;
		if (d < 0) return SIDE_BACK;
		if (d > 0) return SIDE_FRONT;
		return SIDE_ON;
    }

    /**
    * Finds the shortest distance from the parameter point to this plane. If the 
    * point is in front of the plane then the returned distance will be negative, 
    * otherwise it will be positive. Use the fabs() to get the real distance. 
    */
    const double getDistanceFromPlane(const glm::dvec3 & point) const
    {
        return glm::dot(point, normal) - dist;
    }

    /**
    * Finds a point on this plane where the line segment defined by the 
    * parameters lineStart and lineEnd. This point will be passed out on the 
    * parameter intersection. Also this intersect point lies on the line segment, 
    * so if a new line segment is imagined: lineStart to intersection.  The 
    * percent of this new line segment's length in comparison to the first line 
    * segment's length is passed out through the parameter percent. If no 
    * intersection exists between the first line segment and this plane, then the 
    * function returns false, otherwise true. 
    */
    const bool getIntersect(    const glm::dvec3    & lineStart,
                                const glm::dvec3    & lineEnd,
                                glm::dvec3          & intersection,
                                double              & percent) const
    {
        glm::dvec3 direction;

        double lineLength;

        direction = lineEnd - lineStart;

        lineLength = glm::dot(direction, -normal);

        if(fabs(lineLength) < 0.01) { return false ;}

        percent =  getDistanceFromPlane(lineStart) / lineLength;

        if(percent < 0.0f | percent > 1.0f) { return false ;}

        intersection = lineStart + (direction * percent);

        return true;

    }

    /**
    * Returns true if the parameter plane, is exactly facing in the same 
    * direction as this plane. That is their normals have to be equal. Otherwise 
    * returns false.  
    */
    bool sameFacing ( const Plane & plane )
    {
        //return normal.fuzzyCompare(plane.normal, 1e-5f);
        return false;
    }

    /**
    * Returns true if the parameter plane is exactly the same as this plane. 
    * Otherwise returns false.
    */
    bool samePlane ( const Plane & plane )
    {
        return (fabs(plane.dist - dist) < 1e-5f)
            && sameFacing ( plane );

    }

    /**
    * Inverts this plane. That is this plane will divide a space in half exactly 
    * the same as it did before, just the normal of this plane will be facing in 
    * the opposite direction.   
    */
    void invert()
    {
        dist   *= -1.0;
        normal *= -1.0;
    }

    void invertNormal()
    {
        normal *= -1.0;
    }
};

