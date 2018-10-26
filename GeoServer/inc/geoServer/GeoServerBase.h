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

#ifndef __WEB_ASM_PLAY_GEO_SERVER_BASE_H__
#define __WEB_ASM_PLAY_GEO_SERVER_BASE_H__

//#include <utility>
//#include <string>

class GeoServerBase
{
public:
    
    enum
    {
        GET_NUM_GEOMETRIES_REQUEST = 0,
        GET_NUM_GEOMETRIES_RESPONCE,
        GET_GEOMETRY_REQUEST,
        GET_GEOMETRY_RESPONCE,
        GET_LAYER_BOUNDS_REQUEST,
        GET_LAYER_BOUNDS_RESPONCE,
        GET_ALL_GEOMETRIES_REQUEST,
        GET_ALL_GEOMETRIES_RESPONCE,
    };

    //typedef std::pair<unsigned char *, const size_t> WkbGeom;
    //typedef std::pair<char *, const size_t> WkbGeom;
    //typedef std::pair<std::string, const size_t> WkbGeom;

    virtual ~GeoServerBase() {}

protected:

    GeoServerBase() {}
};

#endif // __WEB_ASM_PLAY_GEO_SERVER_BASE_H__