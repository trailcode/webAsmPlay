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

#ifndef __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__

#include <functional>
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

    const std::function<void (const size_t)> callback;

    const size_t ID;
};

class GeoRequestLayerBounds
{
public:

    GeoRequestLayerBounds(const std::function<void (const AABB2D &)> & callback);

private:

    friend class GeoClient;

    const std::function<void (const AABB2D &)> callback;

    const size_t ID;
};

class GeoRequestGeometry
{
public:

    GeoRequestGeometry(const std::function<void (geos::geom::Geometry *)> & callback);

private:

    friend class GeoClient;

    const std::function<void (geos::geom::Geometry *)> callback;

    const size_t ID;
};

class GetRequestGetAllGeometries
{
public:

    GetRequestGetAllGeometries(const std::function<void (std::vector<AttributedGeometry> geoms)> & callback);

private:

    friend class GeoClient;

    const std::function<void (std::vector<AttributedGeometry> geoms)> callback;

    const size_t ID;
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__