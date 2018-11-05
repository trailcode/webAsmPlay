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

#ifndef __WEB_ASM_PLAY_POLYGON_H__
#define __WEB_ASM_PLAY_POLYGON_H__

#include <sstream>
#include <webAsmPlay/Types.h>

namespace geos
{
    namespace geom
    {
        class Polygon;
        class LineString;
        class CoordinateSequence;
    }
}

class Attributes;

class GeometryConverter
{
public:

    static std::string convert(const geos::geom::Polygon * poly, const Attributes * attrs);

    static std::string convert(const AttributedLineString & lineString);

    static AttributedGeometry getGeosPolygon(const char *& poly);

    static AttributedGeometry getGeosLineString(const char *& lineString);

    static std::vector<AttributedGeometry> getGeosPolygons(const char *& polys);

    static std::vector<AttributedGeometry> getGeosLineStrings(const char *& lineStrings);

    static geos::geom::CoordinateSequence * getGeosCoordinateSequence(const char *& lineString);

    static void convert(const geos::geom::Polygon * poly, const Attributes * attrs, std::stringstream & data);

    static void convert(const geos::geom::LineString * lineString, std::stringstream & data);

    static void convert(const AttributedLineString & lineString, std::stringstream & data);

private:

    GeometryConverter() {}
    ~GeometryConverter() {}
};

#endif // __WEB_ASM_PLAY_POLYGON_H__