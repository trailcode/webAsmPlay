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

//#include <OpenSteer/Annotation.h>
#include <OpenSteer/Draw.h>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/Zombie.h>

using namespace OpenSteer;

AVGroup Zombie::neighbors;

namespace
{
    bool gUseDirectedPathFollowing = true; // TODO dup!
    bool gWanderSwitch = true; // TODO dup!

    PolylineSegmentedPathwaySingleRadius * gTestPath = NULL;

    
}

PolylineSegmentedPathwaySingleRadius* getTestPath (void)
{
    if (gTestPath == NULL)
    {
        const float pathRadius = 2;

        const PolylineSegmentedPathwaySingleRadius::size_type pathPointCount = 7;
        const float size = 30;
        const float top = 2 * size;
        const float gap = 1.2f * size;
        const float out = 2 * size;
        const float h = 0.5;
        const Vec3 pathPoints[pathPointCount] =
            {Vec3 (h+gap-out,     0,  h+top-out),  // 0 a
                Vec3 (h+gap,         0,  h+top),      // 1 b
                Vec3 (h+gap+(top/2), 0,  h+top/2),    // 2 c
                Vec3 (h+gap,         0,  h),          // 3 d
                Vec3 (h,             0,  h),          // 4 e
                Vec3 (h,             0,  h+top),      // 5 f
                Vec3 (h+gap,         0,  h+top/2)};   // 6 g

        //gEndpoint0 = pathPoints[0];
        //gEndpoint1 = pathPoints[pathPointCount-1];

        gTestPath = new PolylineSegmentedPathwaySingleRadius (pathPointCount,
                                                                pathPoints,
                                                                pathRadius,
                                                                false);
    }

    return gTestPath;
}

#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/renderables/Renderable.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeosUtil.h>

extern Canvas * theCanvas;

Zombie::Zombie(ProximityDatabase & pd, Network * network)
{
    // allocate a token for this boid in the proximity database
    proximityToken = NULL;

    newPD (pd);

    path = OpenSteerGlue::getPath(network->getRandomPath());

    /*
    dmess("path->pointCount() " << path->pointCount());

    if(path->pointCount() > 2)
    {
        std::vector<glm::dvec2> verts;

        for(size_t i = 0; i < path->pointCount(); ++i)
        {
            Vec3 v = path->point(i);

            verts.push_back(glm::dvec2(v.x, v.z));
        }

        Renderable * r = Renderable::create(geosUtil::getLineString(verts)->buffer(0.0005, 3));

        r->setRenderFill(true);
        r->setRenderOutline(true);

        //theCanvas->addRenderable(r);
    }
    */

    // reset Pedestrian state
    reset ();
}

Zombie::~Zombie()
{
    // delete this boid's token in the proximity database
    delete proximityToken;
}

void Zombie::reset()
{
    // reset the vehicle 
    SimpleVehicle::reset ();

    // max speed and max steering force (maneuverability) 
    setMaxSpeed (2.0);
    setMaxForce (8.0);

    // initially stopped
    setSpeed (0);

    // size of bounding sphere, for obstacle avoidance, etc.
    setRadius (0.5); // width = 0.7, add 0.3 margin, take half

    // set the path for this Pedestrian to follow
    //path = getTestPath ();

    endPoint0 = path->point(0);
    endPoint1 = path->point(path->pointCount() - 1);

    // set initial position
    // (random point on path + random horizontal offset)
    const float d = path->length() * frandom01();
    const float r = path->radius();
    const Vec3 randomOffset = randomVectorOnUnitRadiusXZDisk () * r;
    setPosition (path->mapPathDistanceToPoint (d) + randomOffset);

    // randomize 2D heading
    randomizeHeadingOnXZPlane ();

    // pick a random direction for path following (upstream or downstream)
    pathDirection = (frandom01() > 0.5) ? -1 : +1;

    // trail parameters: 3 seconds with 60 points along the trail
    setTrailParameters (3, 60);

    // notify proximity database that our position has changed
    proximityToken->updateForNewPosition (position());
}

void Zombie::update(const float currentTime, const float elapsedTime)
{
    // apply steering force to our momentum
    applySteeringForce (determineCombinedSteering (elapsedTime),
                        elapsedTime);

    // reverse direction when we reach an endpoint
    if (gUseDirectedPathFollowing)
    {
        const Color darkRed (0.7f, 0, 0);
        float const pathRadius = path->radius();
        
        if (Vec3::distance (position(), endPoint0) < pathRadius )
        {
            pathDirection = +1;
            annotationXZCircle (pathRadius, endPoint0, darkRed, 20);
        }
        if (Vec3::distance (position(), endPoint1) < pathRadius )
        {
            pathDirection = -1;
            annotationXZCircle (pathRadius, endPoint1, darkRed, 20);
        }
    }

    // annotation
    annotationVelocityAcceleration (5, 0);
    recordTrailVertex (currentTime, position());

    // notify proximity database that our position has changed
    proximityToken->updateForNewPosition (position());
}

Vec3 Zombie::determineCombinedSteering(const float elapsedTime)
{
    // move forward
    Vec3 steeringForce = forward();

    // probability that a lower priority behavior will be given a
    // chance to "drive" even if a higher priority behavior might
    // otherwise be triggered.
    const float leakThrough = 0.1f;

    // determine if obstacle avoidance is required
    Vec3 obstacleAvoidance;
    if (leakThrough < frandom01())
    {
        const float oTime = 6; // minTimeToCollision = 6 seconds
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
// just for testing
//             obstacleAvoidance = steerToAvoidObstacles (oTime, gObstacles);
//             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle1);
//             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle3);
        //obstacleAvoidance = steerToAvoidObstacles (oTime, gObstacles);
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
    }

    // if obstacle avoidance is needed, do it
    if (obstacleAvoidance != Vec3::zero)
    {
        steeringForce += obstacleAvoidance;
    }
    else
    {
        // otherwise consider avoiding collisions with others
        Vec3 collisionAvoidance;
        const float caLeadTime = 3;

        // find all neighbors within maxRadius using proximity database
        // (radius is largest distance between vehicles traveling head-on
        // where a collision is possible within caLeadTime seconds.)
        const float maxRadius = caLeadTime * maxSpeed() * 2;
        neighbors.clear();
        proximityToken->findNeighbors (position(), maxRadius, neighbors);

        if (leakThrough < frandom01())
            collisionAvoidance =
                steerToAvoidNeighbors (caLeadTime, neighbors) * 10;

        // if collision avoidance is needed, do it
        if (collisionAvoidance != Vec3::zero)
        {
            steeringForce += collisionAvoidance;
        }
        else
        {
            // add in wander component (according to user switch)
            if (gWanderSwitch)
                steeringForce += steerForWander (elapsedTime);

            // do (interactively) selected type of path following
            const float pfLeadTime = 3;
            const Vec3 pathFollow =
                (gUseDirectedPathFollowing ?
                    steerToFollowPath (pathDirection, pfLeadTime, *path) :
                    steerToStayOnPath (pfLeadTime, *path));

            // add in to steeringForce
            steeringForce += pathFollow * 0.5;
        }
    }

    // return steering constrained to global XZ "ground" plane
    return steeringForce.setYtoZero ();
}

void Zombie::draw()
{
    //drawBasic2dCircularVehicle (*this, gGray50);
    drawBasic2dCircularVehicle (*this, gYellow);
    drawTrail ();
}

void Zombie::annotatePathFollowing(  const Vec3  & future,
                                    const Vec3  & onPath,
                                    const Vec3  & target,
                                    const float   outside)
{
    const Color yellow (1, 1, 0);
    const Color lightOrange (1.0f, 0.5f, 0.0f);
    const Color darkOrange  (0.6f, 0.3f, 0.0f);
    const Color yellowOrange (1.0f, 0.75f, 0.0f);

    // draw line from our position to our predicted future position
    annotationLine (position(), future, yellow);

    // draw line from our position to our steering target on the path
    annotationLine (position(), target, yellowOrange);

    // draw a two-toned line between the future test point and its
    // projection onto the path, the change from dark to light color
    // indicates the boundary of the tube.
    const Vec3 boundaryOffset = (onPath - future).normalize() * outside;
    const Vec3 onPathBoundary = future + boundaryOffset;
    annotationLine (onPath, onPathBoundary, darkOrange);
    annotationLine (onPathBoundary, future, lightOrange);
}

void Zombie::annotateAvoidCloseNeighbor( const AbstractVehicle & other,
                                        const float             additionalDistance)
{
    // draw the word "Ouch!" above colliding vehicles
    const float headOn = forward().dot(other.forward()) < 0;
    const Color green (0.4f, 0.8f, 0.1f);
    const Color red (1, 0.1f, 0);
    const Color color = headOn ? red : green;
    const char* string = headOn ? "OUCH!" : "pardon me";
    const Vec3 location = position() + Vec3 (0, 0.5f, 0);
    //if (OpenSteer::annotationIsOn())
        //draw2dTextAt3dLocation (*string, location, color, drawGetWindowWidth(), drawGetWindowHeight());
}

void Zombie::annotateAvoidNeighbor(  const AbstractVehicle & threat,
                                    const float             steer,
                                    const Vec3            & ourFuture,
                                    const Vec3            & threatFuture)
{
    const Color green (0.15f, 0.6f, 0.0f);

    annotationLine (position(), ourFuture, green);
    annotationLine (threat.position(), threatFuture, green);
    annotationLine (ourFuture, threatFuture, gRed);
    annotationXZCircle (radius(), ourFuture,    green, 12);
    annotationXZCircle (radius(), threatFuture, green, 12);
}

void Zombie::annotateAvoidObstacle(const float minDistanceToCollision)
{
    const Vec3 boxSide = side() * radius();
    const Vec3 boxFront = forward() * minDistanceToCollision;
    const Vec3 FR = position() + boxFront - boxSide;
    const Vec3 FL = position() + boxFront + boxSide;
    const Vec3 BR = position()            - boxSide;
    const Vec3 BL = position()            + boxSide;
    const Color white (1,1,1);
    annotationLine (FR, FL, white);
    annotationLine (FL, BL, white);
    annotationLine (BL, BR, white);
    annotationLine (BR, FR, white);
}

void Zombie::newPD(ProximityDatabase& pd)
{
    // delete this boid's token in the old proximity database
    delete proximityToken;

    // allocate a token for this boid in the proximity database
    proximityToken = pd.allocateToken (this);
}

