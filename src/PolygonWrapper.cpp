#include <glm/vec2.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/PolygonWrapper.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

PolygonWrapper::PolygonWrapper(const Polygon * poly)
{
    convert(poly, data);
}

void PolygonWrapper::convert(const Polygon * poly, stringstream & data)
{
    convert(poly->getExteriorRing(), data);

    const uint32_t numInteriorRings = poly->getNumInteriorRing();

    data.write(reinterpret_cast<const char*>(&numInteriorRings), sizeof(uint32_t));

    for(size_t i = 0; i < numInteriorRings; ++i) { convert(poly->getInteriorRingN(i), data) ;}
}

void PolygonWrapper::convert(const vector<const Polygon *> & polygons, stringstream & data)
{
    const uint32_t numPolygons = polygons.size();

    data.write(reinterpret_cast<const char*>(&numPolygons), sizeof(uint32_t));

    for(size_t i = 0; i < numPolygons; ++i) { convert(polygons[i], data) ;}
}

void PolygonWrapper::writeLineString(const LineString * lineString)
{
    convert(lineString, data);
}

void PolygonWrapper::convert(const LineString * lineString, stringstream & data)
{
    const vector<Coordinate> & coords = *lineString->getCoordinatesRO()->toVector();

    const uint32_t numVerts = coords.size();

    data.write(reinterpret_cast<const char*>(&numVerts), sizeof(uint32_t));

    for(size_t i = 0; i < numVerts; ++i)
    {
        const Coordinate & C = coords[i];

        const double coord[] = { C.x, C.y };
        
        data.write(reinterpret_cast<const char*>(coord), sizeof(coord));
    }
}

CoordinateSequence * PolygonWrapper::getGeosCoordinateSequence(const char *& lineString)
{
    const uint32_t numVerts = *(uint32_t *)lineString; lineString += sizeof(uint32_t);

    vector<Coordinate> * coords = new vector<Coordinate>(numVerts);

    for(size_t i = 0; i < numVerts; ++i)
    {
        const double x = *(double *)lineString; lineString += sizeof(double);
        const double y = *(double *)lineString; lineString += sizeof(double);

        (*coords)[i] = Coordinate(x,y);
    }

    return new CoordinateArraySequence(coords, 2);
}

Polygon * PolygonWrapper::getGeosPolygon(const char *& poly)
{
    const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

    LinearRing * externalRing = factory->createLinearRing(getGeosCoordinateSequence(poly));

    const uint32_t numHoles = *(uint32_t *)poly; poly += sizeof(uint32_t);

    vector<Geometry *> * holes = new vector<Geometry *>(numHoles);

    for(size_t i = 0; i < numHoles; ++i)
    {
        (*holes)[i] = (Geometry *)factory->createLinearRing(getGeosCoordinateSequence(poly));
    }

    return factory->createPolygon(externalRing, holes);
}

vector<Geometry *> PolygonWrapper::getGeosPolygons(const char *& polys)
{
    const uint32_t numPolygons = *(uint32_t *)polys; polys += sizeof(uint32_t);

    vector<Geometry *> ret(numPolygons);

    for(size_t i = 0; i < numPolygons; ++i) { ret[i] = getGeosPolygon(polys) ;}

    return ret;
}

const stringstream & PolygonWrapper::getDataRef() const { return data ;}