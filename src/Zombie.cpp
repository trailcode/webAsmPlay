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

#include <webAsmPlay/Zombie.h>

using namespace OpenSteer;

Zombi::~Zombi()
{

}

void Zombi::reset()
{

}

void Zombi::update(const float currentTime, const float elapsedTime)
{

}

Vec3 Zombi::determineCombinedSteering(const float elapsedTime)
{
    return Vec3(0,0,0);
}

void Zombi::draw()
{

}

void Zombi::annotatePathFollowing(  const Vec3  & future,
                                    const Vec3  & onPath,
                                    const Vec3  & target,
                                    const float   outside)
{

}

void Zombi::annotateAvoidCloseNeighbor( const AbstractVehicle & other,
                                        const float             additionalDistance)
{

}

void Zombi::annotateAvoidNeighbor(  const AbstractVehicle & threat,
                                    const float             steer,
                                    const Vec3            & ourFuture,
                                    const Vec3            & threatFuture)
{

}

void Zombi::annotateAvoidObstacle(const float minDistanceToCollision)
{

}

void Zombi::newPD(ProximityDatabase& pd)
{

}

