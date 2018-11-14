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

#ifndef __WEB_ASM_PLAY_OPEN_STEER_H__
#define __WEB_ASM_PLAY_OPEN_STEER_H__

class Canvas;
class Network;

#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include <geos/geom/Coordinate.h>

namespace OpenSteer
{
    class PolylineSegmentedPathwaySingleRadius;
}

class OpenSteerGlue
{
public:

    static void init(Canvas * canvas, Network * network);

    static const glm::dmat4 & getGeomTrans();
    static const glm::dmat4 & getGeomInverseTrans();

    static size_t setNumZombies(const size_t numZombies);
    static size_t getNumZombies();

    static OpenSteer::PolylineSegmentedPathwaySingleRadius * getPath(const std::unique_ptr<std::vector<geos::geom::Coordinate> > & path);

private:
};

#endif // __WEB_ASM_PLAY_OPEN_STEER_H__