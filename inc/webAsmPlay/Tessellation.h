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

#ifndef __WEB_ASM_PLAY_TESSELLATION_H__
#define __WEB_ASM_PLAY_TESSELLATION_H__

#include <memory>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/Types.h>

class Tessellation
{
public:

    typedef std::vector<std::unique_ptr<const Tessellation> > Tessellations;

    static std::unique_ptr<const Tessellation> tessellatePolygon(const geos::geom::Polygon * poly,
                                                                 const glm::dmat4          & trans,
                                                                 const size_t                symbologyID = 0,
                                                                 const double                height      = 0.0,
                                                                 const double                minHeight   = 0.0);

    static void tessellateMultiPolygon( const geos::geom::MultiPolygon  * multiPoly,
                                        const glm::dmat4                & trans,
                                        Tessellations                   & tessellations,
                                        const size_t                      symbologyID);
    ~Tessellation();

    bool     isEmpty() const;
    uint32_t getSymbologyID() const;
    double   getHeight() const;

private:

    friend class VertexArrayObject;

    double    * verts           = NULL; // TODO make object oriented 
    uint32_t  * triangleIndices = NULL;
    uint32_t    numVerts        = 0;
    uint32_t    numTriangles    = 0;

    Uint32Vec counterVertIndices;
    Uint32Vec lineIndices;

    uint32_t symbologyID;

    double height;
    double minHeight;

private:

    Tessellation(const size_t symbologyID,
                 const double height,
                 const double minHeight);

    Tessellation(const Tessellation &)              = delete;
    Tessellation(Tessellation &&)                   = delete;
    Tessellation & operator=(const Tessellation &)  = delete;
    Tessellation & operator=(Tessellation &)        = delete;
    
};

typedef Tessellation::Tessellations Tessellations;

#endif // __WEB_ASM_PLAY_TESSELLATION_H__