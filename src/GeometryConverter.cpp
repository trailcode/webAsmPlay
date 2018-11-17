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

#include <geos/geom/Polygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Point.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/GeometryConverter.h>

using namespace std;
using namespace glm;
using namespace geos;
using namespace geos::geom;

string GeometryConverter::convert(const AttributedPoligonalArea & polygon)
{
    stringstream ret;

    convert(get<1>(polygon), get<0>(polygon), ret);

    return ret.str();
}

string GeometryConverter::convert(const AttributedLineString & lineString)
{
    stringstream ret;

    convert(lineString, ret);

    return ret.str();
}

string GeometryConverter::convert(const AttributedPoint & point)
{
    stringstream ret;

    convert(point, ret);

    return ret.str();
}

void GeometryConverter::convert(const geom::Polygon * poly, const Attributes * attrs, stringstream & data)
{
    attrs->write(data);

    convert(poly->getExteriorRing(), data);

    const uint32_t numInteriorRings = poly->getNumInteriorRing();

    data.write((const char *)&numInteriorRings, sizeof(uint32_t));

    for(size_t i = 0; i < numInteriorRings; ++i) { convert(poly->getInteriorRingN(i), data) ;}
}

void GeometryConverter::convert(const LineString * lineString, stringstream & data)
{
    const vector<Coordinate> & coords = *lineString->getCoordinatesRO()->toVector();

    const uint32_t numVerts = coords.size();

    data.write((const char *)&numVerts, sizeof(uint32_t));

    for(size_t i = 0; i < numVerts; ++i)
    {
        const Coordinate & C = coords[i];

        const double coord[] = { C.x, C.y };
        
        data.write((const char *)coord, sizeof(coord));
    }
}

void GeometryConverter::convert(const Point * point, stringstream & data)
{
    const double x = point->getX();
    const double y = point->getY();

    data.write((const char *)&x, sizeof(double));
    data.write((const char *)&y, sizeof(double));
}

void GeometryConverter::convert(const AttributedLineString & lineString, stringstream & data)
{
    get<0>(lineString)->write(data);

    convert(get<1>(lineString), data);
}

void GeometryConverter::convert(const AttributedPoint & point, stringstream & data)
{
    get<0>(point)->write(data);

    convert(get<1>(point), data);
}

CoordinateSequence * GeometryConverter::getGeosCoordinateSequence(const char *& lineString)
{
    const uint32_t numVerts = getUint32(lineString);

    vector<Coordinate> * coords = new vector<Coordinate>(numVerts);

    for(size_t i = 0; i < numVerts; ++i) { (*coords)[i] = Coordinate(getDouble(lineString), getDouble(lineString)) ;}

    return new CoordinateArraySequence(coords, 2);
}

AttributedGeometry GeometryConverter::getGeosPolygon(const char *& poly)
{
    Attributes * attrs = new Attributes(poly);

    const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

    LinearRing * externalRing = factory->createLinearRing(getGeosCoordinateSequence(poly));

    const uint32_t numHoles = getUint32(poly);

    vector<Geometry *> * holes = new vector<Geometry *>(numHoles);

    for(size_t i = 0; i < numHoles; ++i) { (*holes)[i] = factory->createLinearRing(getGeosCoordinateSequence(poly)) ;}

    return AttributedGeometry(attrs, factory->createPolygon(externalRing, holes));
}

AttributedGeometry GeometryConverter::getGeosLineString(const char *& lineString)
{
    return AttributedGeometry(new Attributes(lineString),
                              GeometryFactory::getDefaultInstance()->createLineString(getGeosCoordinateSequence(lineString)));
}

AttributedGeometry GeometryConverter::getGeosPoint(const char *& point)
{
    return AttributedGeometry(new Attributes(point),
                              GeometryFactory::getDefaultInstance()->createPoint(Coordinate(getDouble(point), getDouble(point))));
}

vector<AttributedGeometry> GeometryConverter::getGeosPolygons(const char *& polys)
{
    const uint32_t numPolygons = getUint32(polys);

    vector<AttributedGeometry> ret(numPolygons);

    for(size_t i = 0; i < numPolygons; ++i)
    {
        getUint32(polys); // Skip num bytes;

        ret[i] = getGeosPolygon(polys);
    }

    return ret;
}

vector<AttributedGeometry> GeometryConverter::getGeosLineStrings(const char *& lineStrings)
{
    const uint32_t numLineStrings = getUint32(lineStrings);

    vector<AttributedGeometry> ret(numLineStrings);

    for(size_t i = 0; i < numLineStrings; ++i)
    {
        getUint32(lineStrings); // Skip num bytes;

        ret[i] = getGeosLineString(lineStrings);
    }

    return ret;
}

vector<AttributedGeometry> GeometryConverter::getGeosPoints(const char *& points)
{
    const uint32_t numPoints = getUint32(points);

    vector<AttributedGeometry> ret(numPoints);

    for(size_t i = 0; i < numPoints; ++i) { ret[i] = getGeosPoint(points) ;}

    return ret;
}
