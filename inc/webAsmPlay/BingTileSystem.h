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

#include <string>
#include <utility>
#include <glm/vec2.hpp>

// From: https://docs.microsoft.com/en-us/bingmaps/articles/bing-maps-tile-system

namespace bingTileSystem
{
    size_t mapSize(const size_t levelOfDetail);

    double groundResolution(double latitude, const size_t levelOfDetail);

    double mapScale(const double latitude, const size_t levelOfDetail, const size_t screenDpi);

    glm::ivec2 latLongToPixel(const glm::dvec2 & latLong, const size_t levelOfDetail);

    glm::ivec2 latLongToTile(const glm::dvec2 & latLong, const size_t levelOfDetail);

    glm::dvec2 pixelToLatLong(const glm::ivec2 & pixel, const size_t levelOfDetail);

    glm::ivec2 pixelToTile(const glm::ivec2 & pixel);

    glm::ivec2 tileToPixel(const glm::ivec2 & tile);

    glm::dvec2 tileToLatLong(const glm::ivec2 & tile, const size_t levelOfDetail);

    std::string tileToQuadKey(const glm::ivec2 & tile, const size_t levelOfDetail);

    std::pair<size_t, glm::ivec2> quadKeyToTile(const std::string & quadKey);
}
