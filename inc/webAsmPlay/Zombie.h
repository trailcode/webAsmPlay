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

#ifndef __EMSCRIPTEN__
#include <tbb/concurrent_vector.h>
#endif

#include <OpenSteer/SimpleVehicle.h>
#include <OpenSteer/Proximity.h>

class Network;  

namespace OpenSteer
{
    class PolylineSegmentedPathwaySingleRadius;
}

class Zombie : public OpenSteer::SimpleVehicle
{
public:

#ifdef __EMSCRIPTEN__

    typedef std::vector<Zombie *> groupType;

#else

	typedef tbb::concurrent_vector<Zombie*> groupType;

#endif

    typedef OpenSteer::AbstractProximityDatabase        <OpenSteer::AbstractVehicle *> ProximityDatabase;
    typedef OpenSteer::AbstractTokenForProximityDatabase<OpenSteer::AbstractVehicle *> ProximityToken;
    
    Zombie(ProximityDatabase & pd, Network * network);

    virtual ~Zombie();

    // reset all instance state
    void reset();

    //void updateSteering(const float currentTime, const float elapsedTime);

    void update(const float currentTime, const float elapsedTime);

    OpenSteer::Vec3 determineCombinedSteering (const float elapsedTime);

    void draw();

    void annotatePathFollowing( const OpenSteer::Vec3 & future,
                                const OpenSteer::Vec3 & onPath,
                                const OpenSteer::Vec3 & target,
                                const float             outside);

    void annotateAvoidCloseNeighbor(const OpenSteer::AbstractVehicle & other,
                                    const float                        additionalDistance);

    void annotateAvoidNeighbor( const OpenSteer::AbstractVehicle & threat,
                                const float                        steer,
                                const OpenSteer::Vec3            & ourFuture,
                                const OpenSteer::Vec3            & threatFuture);

    void annotateAvoidObstacle(const float minDistanceToCollision);

    void newPD(ProximityDatabase & pd);

    ProximityToken * proximityToken;

    static OpenSteer::AVGroup neighbors;

    OpenSteer::PolylineSegmentedPathwaySingleRadius * path;

    int pathDirection;

    OpenSteer::Vec3 endPoint0;
    OpenSteer::Vec3 endPoint1;

private:
};
