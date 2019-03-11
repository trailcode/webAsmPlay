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

#include <memory>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/Types.h>
#include <geos/geom/Coordinate.h>

namespace geos
{
    namespace geom
    {
        class LineString;
    }
}

class Renderable;
class Attributes;
class GeoClient;

class Edge
{
public:

    Edge(Renderable * renderable, const geos::geom::LineString * geom, Attributes * attributes);

    Renderable * getRenderable() const;

    const geos::geom::LineString * getGeometry() const;

    Attributes * getAttributes() const;

    std::vector<Edge *> neighbors;
    
    //bool visited = false;

    const glm::dvec2 start;
    const glm::dvec2 end;

    inline double getWeight() const { return weight ;}

private:

    Renderable                   * renderable;
    const geos::geom::LineString * geom;
    Attributes                   * attributes;

    int weight;
};

class Network
{
public:

    Network(GeoClient * client = NULL);

    Network(const std::vector<AttributedLineString> & lineStrings, const glm::dmat4 & trans);

    ~Network();

    void setEdges(const std::vector<Edge *> & edges);

    void setEdges(const std::vector<AttributedLineString> & lineStrings);

    void setStartEdge(const PointOnEdge & start);

    void findPath(const PointOnEdge & startPoint);

    std::vector<geos::geom::Coordinate> * findPath(const PointOnEdge & start, const PointOnEdge & end);

    std::vector<glm::dvec2> getRandomPath();

private:

    Renderable * startPosRenderable = NULL;

    GeoClient * client = NULL;

    glm::dmat4 trans;
};
