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
  \copyright 2018
*/
#pragma once

#include <glm/mat4x4.hpp>
#include <webAsmPlay/Types.h>

class Tessellation
{
public:

	typedef std::unique_ptr<const Tessellation> ConstPtr;

    typedef std::vector<ConstPtr> Tessellations;

    static ConstPtr tessellatePolygon(	const geos::geom::Polygon		* poly,
                                                    const glm::dmat4    & trans,
                                                    const size_t          symbologyID = 0,
                                                    const double          height      = 0.0,
                                                    const double          minHeight   = 0.0);

    static void tessellateMultiPolygon( const geos::geom::MultiPolygon  * multiPoly,
                                        const glm::dmat4                & trans,
                                        Tessellations                   & tessellations,
                                        const size_t                      symbologyID);
    ~Tessellation();

    bool     isEmpty() const;
    uint32_t getSymbologyID() const;
    double   getHeight() const;

private:

	Tessellation(	const size_t symbologyID,
					const double height,
					const double minHeight);

	Tessellation(const Tessellation &)              = delete;
	Tessellation(Tessellation &&)                   = delete;
	Tessellation & operator=(const Tessellation &)  = delete;

    friend class VertexArrayObject;

    double    * m_verts           = nullptr; // TODO make object oriented 
    uint32_t  * m_triangleIndices = nullptr;
    uint32_t    m_numVerts        = 0;
    uint32_t    m_numTriangles    = 0;

    Uint32Vec m_counterVertIndices;
    Uint32Vec m_lineIndices;

    uint32_t m_symbologyID;

    double m_height;
    double m_minHeight;
};

typedef Tessellation::Tessellations Tessellations;
typedef Tessellation::ConstPtr		TessellationConstPtr;

