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

#include <webAsmPlay/Types.h>

class GeoClient;

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
}

class GeoRequestGetNumGeoms
{
public:

    GeoRequestGetNumGeoms(const std::function<void (const size_t numGeoms)> & callback);

private:

    friend class GeoClient;

    const std::function<void (const size_t)> m_callback;

    const size_t m_ID;
};

class GeoRequestLayerBounds
{
public:

    GeoRequestLayerBounds(const std::function<void (const AABB2D &)> & callback);

private:

    friend class GeoClient;

    const std::function<void (const AABB2D &)> m_callback;

    const size_t m_ID;
};

class GeoRequestGeometry
{
public:

    GeoRequestGeometry(const std::function<void (geos::geom::Geometry *)> & callback);

private:

    friend class GeoClient;

    const std::function<void (geos::geom::Geometry *)> m_callback;

    const size_t m_ID;
};

class GetRequestGetAllGeometries
{
public:

    GetRequestGetAllGeometries(const std::function<void (std::vector<AttributedGeometry> geoms)> & callback);

private:

    friend class GeoClient;

    const std::function<void (std::vector<AttributedGeometry> geoms)> m_callback;

    const size_t m_ID;
};
