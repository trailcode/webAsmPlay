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

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <algorithm>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/bing/BingTileSystem.h>

using namespace std;
using namespace glm;

// From: https://docs.microsoft.com/en-us/bingmaps/articles/bing-maps-tile-system

namespace
{
    const double a_earthRadius    =  6378137;  
    const double a_minLatitude    = -85.05112878;  
    const double a_maxLatitude    =  85.05112878;  
    const double a_minLongitude   = -180;  
    const double a_maxLongitude   =  180; 

    inline double clip(const double n, const double minValue, const double maxValue)  
    {  
        return min(max(n, minValue), maxValue);  
    }  
}

size_t bingTileSystem::mapSize(const size_t levelOfDetail) { return (size_t)256 << levelOfDetail ;}

double bingTileSystem::groundResolution(double latitude, const size_t levelOfDetail)
{
    latitude = clip(latitude, a_minLatitude, a_maxLatitude);

    return cos(latitude * M_PI / 180) * 2 * M_PI * a_earthRadius / mapSize(levelOfDetail);
}

double bingTileSystem::mapScale(const double latitude, const size_t levelOfDetail, const size_t screenDpi)
{
    return groundResolution(latitude, levelOfDetail) * screenDpi / 0.0254;
}

ivec2 bingTileSystem::latLongToPixel(const dvec2 & latLong, const size_t levelOfDetail)
{
    const double latitude  = clip(latLong.x, a_minLatitude,  a_maxLatitude);
    const double longitude = clip(latLong.y, a_minLongitude, a_maxLongitude);

    const double x = (longitude + 180) / 360;
    const double sinLatitude = sin(latitude * M_PI / 180);
    const double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * M_PI);

    const size_t _mapSize = mapSize(levelOfDetail);  

    return ivec2((int) clip(x * _mapSize + 0.5, 0, double(_mapSize) - 1),
                 (int) clip(y * _mapSize + 0.5, 0, double(_mapSize) - 1));
}

ivec2 bingTileSystem::latLongToTile(const dvec2 & latLong, const size_t levelOfDetail)
{
    return pixelToTile(latLongToPixel(latLong, levelOfDetail));
}

dvec2 bingTileSystem::pixelToLatLong(const ivec2 & pixel, const size_t levelOfDetail)
{
    const double _mapSize = double(mapSize(levelOfDetail));
    const double x = (clip(pixel.x, 0, _mapSize - 1) / _mapSize) - 0.5;  
    const double y = 0.5 - (clip(pixel.y, 0, _mapSize - 1) / _mapSize);  

    return dvec2(90 - 360 * atan(exp(-y * 2 * M_PI)) / M_PI, 360 * x);
}

ivec2 bingTileSystem::pixelToTile(const ivec2 & pixel) { return pixel / ivec2(256, 256) ;}

ivec2 bingTileSystem::tileToPixel(const ivec2 & tile) { return tile * ivec2(256, 256) ;}

dvec2 bingTileSystem::tileToLatLong(const ivec2 & tile, const size_t levelOfDetail)
{
    return pixelToLatLong(tileToPixel(tile), levelOfDetail);
}

string bingTileSystem::tileToQuadKey(const ivec2 & tile, const size_t levelOfDetail)
{
    string quadKey;

    for (int i = int(levelOfDetail); i > 0; i--)  
    {  
        char digit = '0';  
        
        const int mask = 1 << (i - 1);  
        
        if ((tile.x & mask) != 0)  
        {  
            digit++;  
        }

        if ((tile.y & mask) != 0)  
        {  
            digit++;  
            digit++;  
        }

        quadKey += digit;
    }

    return quadKey;
}

pair<size_t, ivec2> bingTileSystem::quadKeyToTile(const string & quadKey)
{
    ivec2 tile;
    
    const size_t levelOfDetail = quadKey.length();

    for (int i = int(levelOfDetail); i > 0; i--)  
    {  
        int mask = 1 << (i - 1);  

        switch (quadKey[levelOfDetail - i])  
        {  
        case '0':  break;  
        case '1':  tile.x |= mask; break;  
        case '2':  tile.y |= mask; break;  

        case '3':  
            tile.x |= mask;  
            tile.y |= mask;  
            break;  

        default:  
            //throw new ArgumentException("Invalid QuadKey digit sequence.");  
            dmess("Error!");
            abort();
        }  
    }

    return make_pair(levelOfDetail, tile);
}
